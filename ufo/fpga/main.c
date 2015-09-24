/*------------------------------------------------------------------
 *  QR
 *
 *
 *------------------------------------------------------------------
 */

//#define DEBUG
#define TRUE 1
#define FALSE 0
#define LOG_LENGTH 5000

#include <stdio.h>
#include <x32.h>
#include "assert.h"
#include "defines.h"
#include <stdint.h>
#include "log.h"

/* define some peripheral short hands
 */
#define X32_instruction_counter           peripherals[0x03]

#define X32_timer_per           peripherals[PERIPHERAL_TIMER1_PERIOD]
#define X32_leds		peripherals[PERIPHERAL_LEDS]
#define X32_ms_clock		peripherals[PERIPHERAL_MS_CLOCK]
#define X32_us_clock		peripherals[PERIPHERAL_US_CLOCK]
#define X32_QR_a0 		peripherals[PERIPHERAL_XUFO_A0]
#define X32_QR_a1 		peripherals[PERIPHERAL_XUFO_A1]
#define X32_QR_a2 		peripherals[PERIPHERAL_XUFO_A2]
#define X32_QR_a3 		peripherals[PERIPHERAL_XUFO_A3]
#define X32_QR_s0 		peripherals[PERIPHERAL_XUFO_S0]
#define X32_QR_s1 		peripherals[PERIPHERAL_XUFO_S1]
#define X32_QR_s2 		peripherals[PERIPHERAL_XUFO_S2]
#define X32_QR_s3 		peripherals[PERIPHERAL_XUFO_S3]
#define X32_QR_s4 		peripherals[PERIPHERAL_XUFO_S4]
#define X32_QR_s5 		peripherals[PERIPHERAL_XUFO_S5]
#define X32_display		peripherals[PERIPHERAL_DISPLAY]
#define X32_QR_timestamp 	peripherals[PERIPHERAL_XUFO_TIMESTAMP]

#define X32_rs232_data		peripherals[PERIPHERAL_PRIMARY_DATA]
#define X32_rs232_stat		peripherals[PERIPHERAL_PRIMARY_STATUS]
#define X32_rs232_char		(X32_rs232_stat & 0x02)

#define X32_wireless_data	peripherals[PERIPHERAL_WIRELESS_DATA]
#define X32_wireless_stat	peripherals[PERIPHERAL_WIRELESS_STATUS]
#define X32_wireless_char	(X32_wireless_stat & 0x02)

#define X32_button		peripherals[PERIPHERAL_BUTTONS]
#define X32_switches		peripherals[PERIPHERAL_SWITCHES]


#define SAFE_MODE_INT		0
#define PANIC_MODE_INT		1
#define MANUAL_MODE_INT		2
#define CALIBRATE_MODE_INT	3
#define YAW_CONTROL_INT		4
#define FULL_CONTROL_INT	5

#define FIFOSIZE 16
uint8_t	fifo[FIFOSIZE]; 
int	iptr, optr;


// Globals
char	c;
int	ALIVE;
int mode;
int	ae[4];
int	s0, s1, s2, s3, s4, s5, timestamp;
int	isr_qr_counter;
int	isr_qr_time;
int	button;
int	inst;

void	toggle_led(int);
void	delay_ms(int);
void	delay_us(int);

/*------------------------------------------------------------------
 * isr_qr_link -- QR link rx interrupt handler
 *------------------------------------------------------------------
 */
void isr_button(void)
{
	button = 1;
}

void isr_led_timer(void) {
	toggle_led(0);
}

/*------------------------------------------------------------------
 * isr_qr_link -- QR link rx interrupt handler
 *------------------------------------------------------------------
 */
void isr_qr_link(void)
{
	int	ae_index;
	/* record time
	 */
	isr_qr_time = X32_us_clock;
        inst = X32_instruction_counter;
	/* get sensor and timestamp values
	 */
	s0 = X32_QR_s0; s1 = X32_QR_s1; s2 = X32_QR_s2; 
	s3 = X32_QR_s3; s4 = X32_QR_s4; s5 = X32_QR_s5;
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
	toggle_led(3);

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
 * getchar -- read char from rx fifo, return -1 if no char available
 *****   @deprecated
 *------------------------------------------------------------------
 */
int getchar(void)
{
	int	c;

	if (optr == iptr)
		return -1;
	c = fifo[optr++];
	if (optr > FIFOSIZE)
		optr = 0;
	return c;
}


uint8_t modecommand;

uint8_t data1;
uint8_t data2;
uint8_t data3;
uint8_t data4;

uint8_t checksum;
uint8_t checker;

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
		}
	else
	{
		//reset buffer pointers if we get out of sync
		optr = iptr = 0;
		return -1;
	}
return 0;
}

/*
	ae0
	$
ae3---ae1
	|
	ae2
*/

void process_packet(void)  //we need to process packet and decide what should be done
{
	if (mode == PANIC_MODE_INT){
		return;	
	}
	
	if ((modecommand == SAFE_MODE) )
		{
//#ifdef DEBUG
		//	printf("********Going to SAFE_MODE!**********\n");
//#endif
			ae[0]=ae[1]=ae[2]=ae[3] = 0;
			mode = SAFE_MODE_INT;
		}
	else if ( (modecommand == PANIC_MODE) && (mode != SAFE_MODE_INT))
		{

//#ifdef DEBUG
			printf("********Going to PANIC_MODE!**********\n");
//#endif
			mode = PANIC_MODE_INT;

		if (ae[0] > 400)
		{
			ae[0] = ae[1] = ae[2] = ae[3] = 300;
			delay_ms(1000);
			ae[0] = ae[1] = ae[2] = ae[3] = 250;
			delay_ms(1000);
		}
//#ifdef DEBUG
			printf("********Engines decreased!**********\n");
//#endif
		if (ae[0] >= 250)
		{
			ae[0] = ae[1] = ae[2] = ae[3] = 200;
			delay_ms(1000);
			ae[0] = ae[1] = ae[2] = ae[3] = 150;
			delay_ms(1000);
		}

		if (ae[0] >= 150)
		{
			ae[0] = ae[1] = ae[2] = ae[3] = 100;
			delay_ms(1000);
			ae[0] = ae[1] = ae[2] = ae[3] = 50;
			delay_ms(1000);
		}
			ae[0]=ae[1]=ae[2]=ae[3] = 0;
//#ifdef DEBUG
			printf("********Going to SAFE_MODE!**********\n");
//#endif
			mode = SAFE_MODE_INT;
		}
	else if (modecommand == MANUAL_MODE)
		{
			mode = MANUAL_MODE_INT;
				//LIFT
				if ( (data1&0x10) == 0x00) //level up only in MANUAL mode
					{
						ae[0]=ae[1]=ae[2]=ae[3]= 65 * (data1&0x0F);
		 			}

		 		//ROLL
		 		if ( (data4&0x10) == 0x00) 
					{
						ae[1]=ae[1] + 15 * (data4&0x0F); //lean left

					if (ae[3] > 30)
						ae[3]=ae[3] - 15 * (data4&0x0F);
		 			}
		 		else
		 			{
		 			if (ae[1] > 30)
						ae[1]=ae[1] - 15 * (data4&0x0F); //lean right
						ae[3]=ae[3] + 15 * (data4&0x0F);
		 			}

				//PITCH
		 		if ( (data3&0x10) == 0x00) 
					{
					if (ae[0] > 30)
						ae[0] = ae[0] - 15 * (data3&0x0F); //lean forward
						ae[2] = ae[2] + 15 * (data3&0x0F); 
		 			}
		 		else
		 			{
						ae[0] = ae[0] + 15 * (data3&0x0F); //lean backward
					if (ae[2] > 30)
						ae[2] = ae[2] - 15 * (data3&0x0F); 
		 			}

		 		//YAW
		 		if ( (data2&0x10) == 0x00) 
					{
						ae[0] = ae[0] + 20 * (data2&0x0F);
						ae[2] = ae[2] + 20 * (data2&0x0F);
						
						if ((ae[1] > 30) && (ae[3] > 30))
						{
							ae[1] = ae[1] - 20 * (data2&0x0F);
							ae[3] = ae[3] - 20 * (data2&0x0F);
						}

		 			}
		 		else
		 		{
						ae[1] = ae[1] + 20 * (data2&0x0F);
						ae[3] = ae[3] + 20 * (data2&0x0F);
			 			if ((ae[0] > 30) && (ae[2] > 30))
						{
			 				ae[0] = ae[0] - 20 * (data2&0x0F);
							ae[2] = ae[2] - 20 * (data2&0x0F);
						}
		 		}
		}

}

/*------------------------------------------------------------------
 * isr_wireless_rx -- wireless rx interrupt handler
 *------------------------------------------------------------------
 */
void isr_wireless_rx(void)
{
	int c;

	/* signal interrupt
	 */
	toggle_led(4);


	/* may have received > 1 char before IRQ is serviced so loop
	 */
	while (X32_wireless_char) {
		fifo[iptr++] = X32_wireless_data;
		if (iptr > FIFOSIZE)
			iptr = 0;
	}

}

/*------------------------------------------------------------------
 * delay_ms -- busy-wait for ms milliseconds
 *------------------------------------------------------------------
 */
void delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms)
		;
}

/*------------------------------------------------------------------
 * delay_us -- busy-wait for us microseconds
 *------------------------------------------------------------------
 */
void delay_us(int us) 
{
	int time = X32_us_clock;
	while(X32_us_clock - time < us)
		;
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
	char text[100] , a;
	printf("%3d %3d %3d %3d | ",ae[0],ae[1],ae[2],ae[3]);
	printf("%3d %3d %3d %3d %3d %3d (%3d, %d)\r\n",
		s0,s1,s2,s3,s4,s5,isr_qr_time, inst);
        
	sprintf(text, "%d %d %d %d \r\n",ae[0],ae[1],ae[2],ae[3]);
    	i = 0;
    	while( text[i] != 0) {
       		delay_ms(1);
		// if (X32_switches == 0x03)
		if (X32_wireless_stat & 0x01 == 0x01)
			X32_wireless_data = text[i];

		i++;
    	}
}

/*------------------------------------------------------------------
 * main -- do the test
 *------------------------------------------------------------------
 */
int main() 
{
	struct LOG log[LOG_LENGTH];
	int log_counter = 0;
	ALIVE = 1;
	mode = SAFE_MODE_INT;

	/* prepare QR rx interrupt handler
	 */
        SET_INTERRUPT_VECTOR(INTERRUPT_XUFO, &isr_qr_link);
        SET_INTERRUPT_PRIORITY(INTERRUPT_XUFO, 21);
	isr_qr_counter = isr_qr_time = 0;
	ae[0] = ae[1] = ae[2] = ae[3] = 0;
        ENABLE_INTERRUPT(INTERRUPT_XUFO);
 	
	/* prepare timer interrupt
	 */
        X32_timer_per = 1000 * CLOCKS_PER_MS;
        SET_INTERRUPT_VECTOR(INTERRUPT_TIMER1, &isr_led_timer);
        SET_INTERRUPT_PRIORITY(INTERRUPT_TIMER1, 21);
        ENABLE_INTERRUPT(INTERRUPT_TIMER1);

	/* prepare button interrupt handler
	 */
        SET_INTERRUPT_VECTOR(INTERRUPT_BUTTONS, &isr_button);
        SET_INTERRUPT_PRIORITY(INTERRUPT_BUTTONS, 8);
	button = 0;
        ENABLE_INTERRUPT(INTERRUPT_BUTTONS);	

	/* prepare rs232 rx interrupt and getchar handler
	 */
        SET_INTERRUPT_VECTOR(INTERRUPT_PRIMARY_RX, &isr_rs232_rx);
        SET_INTERRUPT_PRIORITY(INTERRUPT_PRIMARY_RX, 20);
	while (X32_rs232_char) c = X32_rs232_data; // empty buffer
        ENABLE_INTERRUPT(INTERRUPT_PRIMARY_RX);

        /* prepare wireless rx interrupt and getchar handler
	 */
        SET_INTERRUPT_VECTOR(INTERRUPT_WIRELESS_RX, &isr_wireless_rx);
        SET_INTERRUPT_PRIORITY(INTERRUPT_WIRELESS_RX, 19);
        while (X32_wireless_char) c = X32_wireless_data; // empty buffer
        ENABLE_INTERRUPT(INTERRUPT_WIRELESS_RX);

	/* initialize some other stuff
	 */
        iptr = optr = 0;
		X32_leds = 0;

	/* start the test loop
	 */
        ENABLE_INTERRUPT(INTERRUPT_GLOBAL); 

/*************LOGGING************
**************************/



/*************************
**************************/
    log_counter = 0;
	while (ALIVE)
	{
	//printf("sizeof log struct is: %d\n\n", sizeof(*log) );
		c=get_packet();  //<- possibly add no change packet
		if (c != -1) {
			process_packet();
			 //<-this is a burden
		}
		printf("MODE: %d\n", mode);
		print_state();
/*
	if (mode == SAFE_MODE_INT)
		X32_leds = (X32_leds & 0xFC) | 0x01 );
	else if (mode == PANIC_MODE_INT)
		X32_leds = (X32_leds & 0xFC) | 0x02 );
	else if (mode == MANUAL_MODE_INT)
		X32_leds = (X32_leds & 0xFC) | 0x04 );
*/
    	/*if (log_counter < LOG_LENGTH)
    	{
    		log[log_counter].timestamp = 0xFF00FF00; //timestamp;
    		log[log_counter].ae[0] = 0xFFAA;
    		log[log_counter].ae[1] = 0xFFAA;
    		log[log_counter].ae[2] = 0xFFAA;
    		log[log_counter].ae[3] = 0xFFAA;
    		log[log_counter].s0 = 0;
    		log[log_counter].s1 = 0;
    		log[log_counter].s2 = 0;
    		*/
    		/*log[log_counter].timestamp = timestamp;
    		log[log_counter].ae[0] = ae[0];
    		log[log_counter].ae[1] = ae[1];
    		log[log_counter].ae[2] = ae[2];
    		log[log_counter].ae[3] = ae[3];
    		log[log_counter].s0 = s0;
    		log[log_counter].s1 = s1;
    		log[log_counter].s2 = s2;*/
    		/*
    		log_counter++;
    		printf("*");
    	}
    	else 
    	{
    		printf("\nDONE!!!\n");
    		printf("%p\n", &log );
 			break;
 		}
 		*/
	     //leave this here for now
        /*X32_leds = (X32_leds & 0xFC) | (X32_switches & 0x03 );
		*/
		/*if (button == 1){
			printf("You have pushed the button!!!\r\n");
			button = 0;
		}*/

	}


	//Possible put the critical mode and safe mode here, such that
	//if we break from main loop we start with these modes straight away

	printf("Exit\r\n");

        DISABLE_INTERRUPT(INTERRUPT_GLOBAL);

	return 0;
}