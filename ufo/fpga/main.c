/*------------------------------------------------------------------
 *  QR
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <x32.h>
#include "assert.h"
#include <stdint.h>
#include <stdlib.h>
#include "log.h"
#include "defines.h"
#include "../modules/pkt/pkt.h"

uint8_t	fifo[FIFOSIZE]; 
int	iptr = 0; 
int optr = 0;

// 
struct LOG log[LOG_LENGTH];
char c;
int	ALIVE = 1;
int mode = SAFE_MODE_INT;
int	ae[4];

int	sax, say, saz, sp, sq, sr, timestamp;
int	sax0, say0, saz0, sp0, sq0, sr0; //Callibration offsets

int	isr_qr_counter;
int	isr_qr_time;
int	button;
int	inst;

// Logging
int log_sent = 0;
int log_counter = 0;

int lift_setpoint = 0;
int lift_setpoint_rpm = 0;

//filter & yaw control
int zr_old = 0;
int zr_filtered = 0;
int zr_filtered_old = 0;
int a0 = 1;
unsigned int a1 ;  //0.0305;
unsigned int b1 ;  //0.0305;
	int zr_v;


int yaw;
int calibrated = FALSE;
int yaw_control_loop = FALSE;

uint8_t yaw_p = 1;
uint8_t full_p1 = 1;
uint8_t full_p2 = 1;

uint8_t sensitivity = 1;


//packet processing global variables
uint8_t modecommand;
uint8_t data1;
uint8_t data2;
uint8_t data3;
uint8_t data4;
uint8_t checksum;
uint8_t checker;

int startTimestamp, endTimestamp, counter;

void 	delay_ms(int ms);
void 	delay_us(int us);
void	epileptic_delay_ms(int ms);
void 	toggle_led(int i);
void 	print_state(void);

void 	calibrate(void);
int 	zax(void)	{return sax - sax0;}
int 	zay(void)	{return say - say0;}
int 	zaz(void)	{return saz - saz0;}
int 	zp(void)	{return sp - sp0;}
int 	zq(void)	{return sq - sq0;}
int 	zr(void)	{return sr - sr0;}

void 	periodic(void);
void 	isr_qr_link(void);
void 	isr_rs232_rx(void);

void	move_optr();
int 	get_packet(void);
void	process_packet(void);

// QR behaviour
void 	panic();
void	logs_send();
int within_bounds(int x, int lower_limit, int upper_limit);

/*------------------------------------------------------------------
 * main loop
 *------------------------------------------------------------------
 */

int main() 
{
	int timestamp_alive_led_toggle = 0;
	int timestamp_last_pkt = 0;
	int count = 1;

	sax = say = saz = sp = sq = sr = 0;

	/* prepare QR rx interrupt handler
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_qr_link);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, 24);
	isr_qr_counter = isr_qr_time = 0;
	ae[0] = ae[1] = ae[2] = ae[3] = 0;
	ENABLE_INTERRUPT(INTERRUPT_XUFO);

	/* prepare timer interrupt #1
	*/
	X32_timer_per = 1 * CLOCKS_PER_MS;
	SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &periodic);
	SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, 22);
	ENABLE_INTERRUPT(INTERRUPT_TIMER1);

	/* prepare rs232 rx interrupt and getchar handler
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rs232_rx);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, 20);
	while (X32_rs232_char) c = X32_rs232_data; // empty buffer
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

	/* initialize some other stuff
	*/
	X32_leds = 0;

	ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	while (ALIVE)
	{
		c = get_packet();  //<- possibly add no change packet
		if (c != -1) {
			process_packet();
			timestamp_last_pkt = X32_ms_clock;
		}

		// Toogle alive led
	 	if(X32_ms_clock - timestamp_alive_led_toggle > 1000) {
			toggle_led(LED_ALIVE);
			timestamp_alive_led_toggle = X32_ms_clock;
		}  

		// we have lost communication to the qr -> panic
		if ((X32_ms_clock - timestamp_last_pkt) > THRESHOLD && timestamp_alive_led_toggle != 0) {	
			panic();
			timestamp_last_pkt = 0;
		}
		PRINT_STATE(250);

	}
	printf("Exit\r\n");
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	// If for some reason the qr is not in safe/panic mode -> panic 
	if(mode != PANIC_MODE_INT || mode != SAFE_MODE_INT) {
		panic();
	} 

	return 0;
}

void process_packet(void)  //we need to process packet and decide what should be done
{
	if (mode == PANIC_MODE_INT){
		return;	
	}

	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	if ((modecommand == SAFE_MODE) )
		{
			SET_ALL_ENGINE_RPM(0);
			mode = SAFE_MODE_INT;
		}
	else if ( (modecommand == PANIC_MODE) && (mode != SAFE_MODE_INT))
		{
			panic();
		}
	else if ((modecommand == YAW_CONTROL) && (calibrated == TRUE))
		{
			mode = YAW_CONTROL_INT;
			//check for P value changes
			printf("\n %x \n", data1&0xE0);
			if ((data1&0xE0) == 0xE0)
			{
				if (yaw_P < 20)	yaw_P ++;
			}
			else if ((data1&0xE0) == 0xA0)
			{
				if (yaw_P > 1)	yaw_P --;
			}
			
			//LIFT
			if ( (data1&0x10) == 0x00)
				{
					//modify engine values if lift is changed!
					//DND the yaw control
					if (lift_setpoint != (int)data1&0x0F)
					{
						lift_setpoint = (int)(data1&0x0F);
						lift_setpoint_rpm = lift_setpoint * 65;
						SET_ALL_ENGINE_RPM(lift_setpoint_rpm);
						yaw_control_loop = lift_setpoint ? TRUE : FALSE;
					}
				}

			//ROLL
			if ( (data4&0x10) == 0x00) 
				{
					ae[1]=ae[1] + 15 * (data4&0x0F); //lean left
					if (ae[3] - 15 * (data4&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[3]=ae[3] - 15 * (data4&0x0F);
				}
			else
				{
					ae[3]=ae[3] + 15 * (data4&0x0F);
					if (ae[1] - 15 * (data4&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[1]=ae[1] - 15 * (data4&0x0F); //lean right
				}

			//PITCH
			if ( (data3&0x10) == 0x00) 
				{
					ae[2] = ae[2] + 15 * (data3&0x0F); 
					if (ae[0] - 15 * (data3&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[0] = ae[0] - 15 * (data3&0x0F); //lean forward
				}
			else
				{
					ae[0] = ae[0] + 15 * (data3&0x0F); //lean backward
					if (ae[2] - 15 * (data3&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[2] = ae[2] - 15 * (data3&0x0F); 
				}

			//YAW in CONTROL LOOP
			//set the yaw rate variable that is used in the control loop
			if ( (data2&0x10) == 0x00) 
				{
					yaw  = data2&0x0F;
				}
			else
				{
					yaw = -1 * data2&0x0F;
				}
		}
	else if ((modecommand == YAW_CONTROL) && (calibrated == FALSE))
		{
			printf("$QR must be calibrated first! \n");
		}
	else if (modecommand == MANUAL_MODE)
		{
			mode = MANUAL_MODE_INT;
			//LIFT
			if ( (data1&0x10) == 0x00) //level up only in MANUAL mode
				{
					SET_ALL_ENGINE_RPM(65 * (data1&0x0F));
				}

			//ROLL
			if ( (data4&0x10) == 0x00) 
				{
					ae[1]=ae[1] + 15 * (data4&0x0F); //lean left
					if (ae[3] - 15 * (data4&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[3]=ae[3] - 15 * (data4&0x0F);
				}
			else
				{
					ae[3]=ae[3] + 15 * (data4&0x0F);
					if (ae[1] - 15 * (data4&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[1]=ae[1] - 15 * (data4&0x0F); //lean right
				}

			//PITCH
			if ( (data3&0x10) == 0x00) 
				{
					ae[2] = ae[2] + 15 * (data3&0x0F); 
					if (ae[0] - 15 * (data3&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[0] = ae[0] - 15 * (data3&0x0F); //lean forward
				}
			else
				{
					ae[0] = ae[0] + 15 * (data3&0x0F); //lean backward
					if (ae[2] - 15 * (data3&0x0F) > MINIMUM_ENGINE_SPEED)
						ae[2] = ae[2] - 15 * (data3&0x0F); 
				}

			//YAW
			if ( (data2&0x10) == 0x00) 
				{
					ae[0] = ae[0] + 25 * (data2&0x0F);
					ae[2] = ae[2] + 25 * (data2&0x0F);
					
					if ((ae[1] - 25 * (data2&0x0F) > MINIMUM_ENGINE_SPEED) && (ae[3] - 25 * (data2&0x0F) > MINIMUM_ENGINE_SPEED))
					{
						ae[1] = ae[1] - 25 * (data2&0x0F);
						ae[3] = ae[3] - 25 * (data2&0x0F);
					}

				}
			else
			{
				ae[1] = ae[1] + 25 * (data2&0x0F);
				ae[3] = ae[3] + 25 * (data2&0x0F);
				if ((ae[0] - 25 * (data2&0x0F) > MINIMUM_ENGINE_SPEED) && (ae[2] - 25 * (data2&0x0F)  > MINIMUM_ENGINE_SPEED))
				{
					ae[0] = ae[0] - 25 * (data2&0x0F);
					ae[2] = ae[2] - 25 * (data2&0x0F);
				}
	 		}
		}

		else if ( (modecommand == SEND_TELEMETRY) && (mode == SAFE_MODE_INT) && !log_sent ) 
		{
			logs_send();
		}
		else if ( (modecommand == CALIBRATE_MODE) && (mode == SAFE_MODE_INT) )
		{
			delay_ms(1000);
			calibrate();
			printf("$QR calibrated...\n");
			toggle_led(LED_OTHER);
			delay_ms(500);
			toggle_led(LED_OTHER);
			delay_ms(500);
			toggle_led(LED_OTHER);

			calibrated = TRUE;
		}
		else if (modecommand==P_VALUES_MODE) //((mode==YAW_CONTROL_INT) || (mode==FULL_CONTROL_INT))  &&  
		{
			yaw_p 	= within_bounds(data1,1,20);
			full_p1 = within_bounds(data2,1,20);
			full_p2 = within_bounds(data3,1,20);
			sensitivity = within_bounds(data4,1,20);
		}
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

void logs_send() {
	int i = 0;
	#ifdef LOGGING
		printf("********SENDING LOG DATA!**********\n");
		for (i=0; i < log_counter; i++)
		{
			printf("%d ", log[i].timestamp );
			printf("%d ", log[i].ae[0] );
			printf("%d ", log[i].ae[1] );
			printf("%d ", log[i].ae[2] );
			printf("%d ", log[i].ae[3] );
			printf("%d ", log[i].s[0] );
			printf("%d ", log[i].s[1] );
			printf("%d ", log[i].s[2] );
			printf("%d ", log[i].s[3] );
			printf("%d ", log[i].s[4] );
			printf("%d ", log[i].s[5] );
			printf("%d ", log[i].lift_point );
			printf("\n");
		}
		printf("\n");
		printf("$"); //signal end of transmission
		log_sent = 1;
	#endif
	#ifndef LOGGING
		printf("$Failed to send logs, because logging was not enabled on quadcopter.\n");
	#endif
}

void panic() {
	mode = PANIC_MODE_INT;
	printf("$********Going to PANIC_MODE!********\n");
	if (ae[0] > 400)
	{
		SET_ALL_ENGINE_RPM(300);
		epileptic_delay_ms(500);
		SET_ALL_ENGINE_RPM(250);
		epileptic_delay_ms(500);
	}
	printf("$********Engines decreased!**********\n");
	print_state();
	if (ae[0] >= 250)
	{
		SET_ALL_ENGINE_RPM(200);
		epileptic_delay_ms(500);
		SET_ALL_ENGINE_RPM(150);
		epileptic_delay_ms(500);
	}
	printf("$********Engines decreased!**********\n");
	print_state();
	if (ae[0] >= 150)
	{
		SET_ALL_ENGINE_RPM(100);
		epileptic_delay_ms(500);
		SET_ALL_ENGINE_RPM(50);
		epileptic_delay_ms(500);
	}
	SET_ALL_ENGINE_RPM(0);
	printf("$********Going to SAFE MODE!*********\n");
	mode = SAFE_MODE_INT;
}

/*------------------------------------------------------------------
 * Calibrate the sensors
 *------------------------------------------------------------------
*/
void calibrate(void)
{
	int i;
	DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	sax0 = say0 = saz0 = sp0 = sq0 = sr0 = 0;
	for (i=0; i<128; i++)
	{
		sax0 += sax;
		say0 += say;
		saz0 += saz;
		sp0 += sp;
		sq0 += sq;
		sr0 += sr;
		delay_ms(2);
	}

	sax0 = sax0 >> 7;
	say0 = say0 >> 7;
	saz0 = saz0 >> 7;
	sp0 = sp0 >> 7;
	sq0 = sq0 >> 7;
	sr0 = sr0 >> 7;
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
}

/*------------------------------------------------------------------
 * Periodic execution via Timer1 interrupt
 *------------------------------------------------------------------
*/

void periodic(void) {
		if ((mode == YAW_CONTROL_INT) && (yaw_control_loop == TRUE))
		{
			DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
			zr_v = zr();
			
		    //zr_filtered = (a0 * zr) + (a1 * zr_old) - (b1 * zr_filtered_old);
			//zr_old = zr;
			//zr_filtered_old = zr_filtered;
			
			//#define DEBUG
			#ifdef DEBUG
			printf("zr is %d   yaw is %d    yap_P is %d \n", zr_v, yaw, yaw_p);
			#endif

			ae[0] = lift_setpoint_rpm + (yaw - zr_v) * yaw_p;
			ae[2] = ae[0];
			ae[1] = lift_setpoint_rpm - (yaw - zr_v) * yaw_p;
			ae[3] = ae[1];
			ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
		}
}

/*------------------------------------------------------------------
 * isr_qr_link -- QR link rx interrupt handler
 * This function is executed at 1270Hz
 * Logging can be done here as well
 *------------------------------------------------------------------
 */
void isr_qr_link(void) //1270 Hz
{
	int	ae_index;
	/* record time
	 */
	isr_qr_time = X32_us_clock;
        inst = X32_instruction_counter;
	/* get sensor and timestamp values
	 */
	sax = X32_QR_s0; say = X32_QR_s1; saz = X32_QR_s2; 
	sp = X32_QR_s3; sq = X32_QR_s4; sr = X32_QR_s5;
	timestamp = X32_QR_timestamp;

	/* monitor presence of interrupts 
	 */
	isr_qr_counter++;
	if (isr_qr_counter % 500 == 0) {
		toggle_led(LED_QR);
	}	

	/* Clip engine values to be positive and 10 bits.
	 */
	for (ae_index = 0; ae_index < 4; ae_index++) 
	{
		if (ae[ae_index] < 0) 
			ae[ae_index] = 0;
		
		ae[ae_index] &= 0x3ff;
	}

	/* Send actuator values
	 * (Need to supply a continous stream, otherwise
	 * QR will go to safe mode, so just send every ms)
	 */
	X32_QR_a0 = ae[0];
	X32_QR_a1 = ae[1];
	X32_QR_a2 = ae[2];
	X32_QR_a3 = ae[3];

	/* record isr execution time (ignore overflow)
	 */
	inst = X32_instruction_counter - inst;
	isr_qr_time = X32_us_clock - isr_qr_time;


	/*
	* Logging
	*/
	#ifdef LOGGING
		if ((log_counter < LOG_LENGTH) && (mode == YAW_CONTROL_INT) ) {
			log[log_counter].timestamp = X32_ms_clock; 
			log[log_counter].ae[0] = (uint16_t) ae[0];
			log[log_counter].ae[1] = (uint16_t) ae[1];
			log[log_counter].ae[2] = (uint16_t) ae[2];
			log[log_counter].ae[3] = (uint16_t) ae[3];
			log[log_counter].s[0] = sax;
			log[log_counter].s[1] = say;
			log[log_counter].s[2] = saz;
			log[log_counter].s[3] = sp;
			log[log_counter].s[4] = sq;
			log[log_counter].s[5] = sr;
			log[log_counter].lift_point = lift_setpoint_rpm;
			log_counter++;
		}
	#endif
}

/*------------------------------------------------------------------
 * isr_rs232_rx -- rs232 rx interrupt handler
 *------------------------------------------------------------------
 */
void isr_rs232_rx(void) {
	toggle_led(LED_RS232);

	/* may have received > 1 char before IRQ is serviced so loop
	 */
	while (X32_rs232_char) {
		fifo[iptr++] = X32_rs232_data;
		if (iptr >= FIFOSIZE)
			iptr = 0;
	}
}


/*------------------------------------------------------------------
 * get_packet -- construct packet. return -1 on failure.
 *------------------------------------------------------------------
 */

void move_optr() {	
	if (optr == FIFOSIZE-1)
		optr = 0;
	else 
		optr++;
}

int get_packet(void)
{
	uint8_t	c;
	if (optr == iptr) //nothing to process
		return -1;

	c = (uint8_t)fifo[optr++];
	if (c == HEADER) //start of the packet
		{
			fifo[optr-1] = 0x00; //corrupt the header, otherwise we get into loops later
			if (optr==FIFOSIZE) optr=0;
			modecommand	= (uint8_t)fifo[optr];
			move_optr();
			data1 	=	(uint8_t)fifo[optr];
			move_optr();
			data2 	=	(uint8_t)fifo[optr];
			move_optr();
			data3 	=	(uint8_t)fifo[optr];
			move_optr();
			data4 	=	(uint8_t)fifo[optr];
			move_optr();
			checksum =	(uint8_t)fifo[optr];
			move_optr();
			checker = modecommand ^ data1 ^ data2 ^ data3 ^ data4 ^ checksum;
			//hack, because we shouldn't be getting this error. it somehow gets out of sync
			if (iptr != optr) return -1;

			#ifdef DEBUG
				printf("\niptr is: %d,  optr id: %d \n", iptr, optr);
				printf("mode is: %x \n", modecommand);
				printf("LIFT is: %x \n", data1);
				printf("YAW is: %x \n", data2);
				printf("PITCH is: %x \n", data3);
				printf("ROLL is: %x \n", data4);
				printf("Checksum is: %x \n", checksum);
				printf("%s\n", checker==0 ? "PASS" : "FAIL");
			#endif
			//check checksum
			if ( (int)checker != 0) {
				return -1; //ERROR, invalid packet
			}
		}
	else
	{
		//reset buffer pointers if we get out of sync
		optr = iptr = 0;
		return -1;
	}
	return 0;
}


/*------------------------------------------------------------------
 * delay_ms -- busy-wait for ms milliseconds
 *------------------------------------------------------------------
 */
void delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms);
}


void epileptic_delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms) {
		if(X32_ms_clock % 20 == 0) {
			toggle_led(LED_OTHER);
			toggle_led(LED_ALIVE);
		}
	}
}

/*------------------------------------------------------------------
 * delay_us -- busy-wait for us microseconds
 *------------------------------------------------------------------
 */
void delay_us(int us) 
{
	int time = X32_us_clock;
	while(X32_us_clock - time < us);
}

/*------------------------------------------------------------------
 * toggle_led -- toggle led # i
 *------------------------------------------------------------------
 */
void toggle_led(int i) 
{
	X32_leds = (X32_leds ^ (1 << i));
}


/*------------------------------------------------------------------
 * print_state -- print all sensors and actuators
 *------------------------------------------------------------------
 */
void print_state(void) 
{
	printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
	printf("%3d %3d %3d %3d %3d %3d | %d %d %d %d \r\n",
		zax(),zay(),zaz(),zp(),zq(),zr(), yaw_p, full_p1, full_p2, sensitivity);
}

int within_bounds(int x, int lower_limit, int upper_limit) {
	if(x > upper_limit) {
		return upper_limit;
	}
	if(x < lower_limit) {
		return lower_limit;
	}
	return x;
}