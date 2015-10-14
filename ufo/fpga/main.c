/*------------------------------------------------------------------
 *  QR
 *
 *------------------------------------------------------------------
 */

#include <stdio.h>
#include <x32.h>
#include "assert.h"
#include <stdint.h>
#include <stdlib.h>
#include "log.h"
#include "defines.h"

uint8_t	fifo[FIFOSIZE]; 
int	iptr, optr;
int TERM_CONNECTED = 0; //communication safety mechanism

// 
struct LOG log[LOG_LENGTH];
char	c;
int	ALIVE;
int mode;
int	ae[4];

int	sax, say, saz, sp, sq, sr, timestamp;
//Callibration offsets
int	sax0, say0, saz0, sp0, sq0, sr0;

int	isr_qr_counter;
int	isr_qr_time;
int	button;
int	inst;
int log_sent = 0;
int log_counter;

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

int yaw_P = 1;
int yaw;
int calibrated = FALSE;
int YAW_CONTROL_LOOP = FALSE;

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

/*------------------------------------------------------------------
 * main loop
 *------------------------------------------------------------------
 */

int main() 
{
	int timer1;
	int timer2;
	int timestamp_alive_led_toggle = 0;
	int count = 1;


	ALIVE = 1;
	mode = SAFE_MODE_INT;
	sax = say = saz = sp = sq = sr = 0;

	/* prepare QR rx interrupt handler
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_qr_link);
	SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, 21);
	isr_qr_counter = isr_qr_time = 0;
	ae[0] = ae[1] = ae[2] = ae[3] = 0;
	ENABLE_INTERRUPT(INTERRUPT_XUFO);

	/* prepare timer interrupt #1
	*/
	X32_timer_per = 1 * CLOCKS_PER_MS;
	SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &periodic);
	SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, 5);
	ENABLE_INTERRUPT(INTERRUPT_TIMER1);

	/* prepare rs232 rx interrupt and getchar handler
	*/
	SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rs232_rx);
	SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, 20);
	while (X32_rs232_char) c = X32_rs232_data; // empty buffer
	ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

	/* initialize some other stuff
	*/
	iptr = optr = 0;
	X32_leds = 0;
	log_counter = 0;

	ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

	while (ALIVE)
	{
		c=get_packet();  //<- possibly add no change packet
		if (c != -1) {
			process_packet();
			timer1 = X32_ms_clock; //<- maybe its better to move this into the get_packet()
		}

		// Toogle alive led
	 	if(X32_ms_clock - timestamp_alive_led_toggle > 1000) {
			toggle_led(0);
			timestamp_alive_led_toggle = X32_ms_clock;
		}  

		timer2 = X32_ms_clock;
		if (((timer2-timer1) > THRESHOLD) && TERM_CONNECTED)
			{	
				PANIC_AND_EXIT;
			}

		PRINT_STATE(250);

	}//end of main loop

	printf("Exit\r\n");
    DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
	return 0;
}

void process_packet(void)  //we need to process packet and decide what should be done
{
	int log_counter; //is this efficient?

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
			printf("$********Going to PANIC_MODE!**********\n");
			mode = PANIC_MODE_INT;
			if (ae[0] > 400)
			{
				SET_ALL_ENGINE_RPM(300);
				delay_ms(500);
				SET_ALL_ENGINE_RPM(250);
				delay_ms(500);
			}
			printf("$********Engines decreased!**********\n");
			if (ae[0] >= 250)
			{
				SET_ALL_ENGINE_RPM(200);
				delay_ms(500);
				SET_ALL_ENGINE_RPM(150);
				delay_ms(500);
			}
			if (ae[0] >= 150)
			{
				SET_ALL_ENGINE_RPM(100);
				delay_ms(500);
				SET_ALL_ENGINE_RPM(50);
				delay_ms(500);
			}
			SET_ALL_ENGINE_RPM(0);
			printf("$********Going to SAFE MODE!**********\n");
			mode = SAFE_MODE_INT;
		}
	else if ((modecommand == YAW_CONTROL) && (calibrated == TRUE))
		{
			mode = YAW_CONTROL_INT;

			//check for P value changes
			printf("\n %x \n", data1&0xE0);
			if ((data1&0xE0) == 0xE0)
			{
				//printf("\n Incrementing YAW P \n");
				if (yaw_P < 20)	yaw_P ++;
			}
			else if ((data1&0xE0) == 0xA0)
			{
				//printf("\n Decrementing YAW P \n");
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
						if (lift_setpoint > 2) 	YAW_CONTROL_LOOP = TRUE;
						else YAW_CONTROL_LOOP = FALSE;
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
			#ifdef LOGGING
				printf("********SENDING LOG DATA!**********\n");
				for (log_counter=0; log_counter < LOG_LENGTH; log_counter++)
				{
					printf("%d ", log[log_counter].timestamp );
					printf("%d ", log[log_counter].ae[0] );
					printf("%d ", log[log_counter].ae[1] );
					printf("%d ", log[log_counter].ae[2] );
					printf("%d ", log[log_counter].ae[3] );
					printf("%d ", log[log_counter].s[0] );
					printf("%d ", log[log_counter].s[1] );
					printf("%d ", log[log_counter].s[2] );
					printf("%d ", log[log_counter].s[3] );
					printf("%d ", log[log_counter].s[4] );
					printf("%d ", log[log_counter].s[5] );
					printf("%d ", log[log_counter].lift_point );
					printf("\n");

				}
				printf("\n");
				printf("$"); //signal end of transmission
				log_sent = 1;
			#endif
		}
		else if ( (modecommand == CALIBRATE_MODE) && (mode == SAFE_MODE_INT) )
		{
			delay_ms(1000);
			calibrate();
			printf("$QR calibrated...\n");
			toggle_led(3);
			delay_ms(500);
			toggle_led(3);
			delay_ms(500);
			toggle_led(3);

			calibrated = TRUE;
		}
	ENABLE_INTERRUPT(INTERRUPT_GLOBAL);
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
		if ((mode == YAW_CONTROL_INT) && (YAW_CONTROL_LOOP == TRUE))
		{
			DISABLE_INTERRUPT(INTERRUPT_GLOBAL);
			zr_v = zr();
			
		    //zr_filtered = (a0 * zr) + (a1 * zr_old) - (b1 * zr_filtered_old);
			//zr_old = zr;
			//zr_filtered_old = zr_filtered;
			
			//#define DEBUG
			#ifdef DEBUG
			printf("zr is %d   yaw is %d    yap_P is %d \n", zr_v, yaw, yaw_P);
			#endif

			ae[0] = lift_setpoint_rpm + (yaw - zr_v) * yaw_P;
			ae[2] = ae[0];
			ae[1] = lift_setpoint_rpm - (yaw - zr_v) * yaw_P;
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
		toggle_led(2);
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
		log[log_counter].timestamp = X32_ms_clock; //should be replaced with timestamp
		log[log_counter].ae[0] = (uint16_t) ae[0];
		log[log_counter].ae[1] = (uint16_t) ae[1];
		log[log_counter].ae[2] = (uint16_t) ae[2];
		log[log_counter].ae[3] = (uint16_t) ae[3];
		log[log_counter].s[0] = sax;  //should we log these RAW or callibrated values?
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
void isr_rs232_rx(void)
{
	int	c;

	/* signal interrupt
	 */
	toggle_led(1);

	/* may have received > 1 char before IRQ is serviced so loop
	 */
	while (X32_rs232_char) {
		fifo[iptr++] = X32_rs232_data;
#ifdef DEBUG
printf("ISR uart: iptr: %d", iptr-1);		
printf(" => %x\n", fifo[iptr-1]);
#endif
		if (iptr >= FIFOSIZE)
			iptr = 0;
	}

}


/*------------------------------------------------------------------
 * get_packet -- construct packet. return -1 on failure.
 *------------------------------------------------------------------
 */

void move_optr()
{	
	if (optr == FIFOSIZE-1)
		optr = 0;
	else optr++;
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
			//#define DEBUG
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
			if ( (int)checker != 0)
			{
				#ifdef DEBUG
				printf("Invalid packet recieved! Discarding!\n");
				#endif
			 return -1; //ERROR, invalid packet
			}
			else if (TERM_CONNECTED == 0); {  //a check for communication safety mechanism
				TERM_CONNECTED = 1;  //maybe we can move this somewhere else
			}						//such that we do this check only once
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
	int i;
	//char text[100] , a;
	printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
	//printf("%3d %3d %3d %3d %3d %3d (%3d, %d)\r\n",
		//sax,say,say,sp,sq,sr,isr_qr_time, inst);

	/*printf("%3d %3d %3d %3d %3d %3d | %d | (%3d, %d)\r\n",
		zax(),zay(),zaz(),zp(),zq(),zr(), yaw_P, isr_qr_time, inst);
		*/

	printf("%3d %3d %3d %3d %3d %3d | %d\r\n",
		zax(),zay(),zaz(),zp(),zq(),zr(), yaw_P);

    //wireless transmission
    /*  
	sprintf(text, "%d %d %d %d \r\n",ae[0],ae[1],ae[2],ae[3]);
	i = 0;
	while( text[i] != 0) {
		delay_ms(1);
		// if (X32_switches == 0x03)
		if (X32_wireless_stat & 0x01 == 0x01)
			X32_wireless_data = text[i];
		i++;
	}
    */
}