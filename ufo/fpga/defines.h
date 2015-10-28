#ifndef DEFINES_H_
#define DEFINES_H_
/*
 * Header file containing all of the constants and shorthands for the applciation
 */

// define some peripheral short hands
#define X32_instruction_counter     peripherals[0x03]
#define X32_timer_per				peripherals[PERIPHERAL_TIMER1_PERIOD]
#define X32_leds					peripherals[PERIPHERAL_LEDS]
#define X32_ms_clock				peripherals[PERIPHERAL_MS_CLOCK]
#define X32_us_clock				peripherals[PERIPHERAL_US_CLOCK]
#define X32_QR_a0 					peripherals[PERIPHERAL_XUFO_A0]
#define X32_QR_a1 					peripherals[PERIPHERAL_XUFO_A1]
#define X32_QR_a2 					peripherals[PERIPHERAL_XUFO_A2]
#define X32_QR_a3 					peripherals[PERIPHERAL_XUFO_A3]
#define X32_QR_s0 					peripherals[PERIPHERAL_XUFO_S0]
#define X32_QR_s1 					peripherals[PERIPHERAL_XUFO_S1]
#define X32_QR_s2 					peripherals[PERIPHERAL_XUFO_S2]
#define X32_QR_s3 					peripherals[PERIPHERAL_XUFO_S3]
#define X32_QR_s4 					peripherals[PERIPHERAL_XUFO_S4]
#define X32_QR_s5 					peripherals[PERIPHERAL_XUFO_S5]
#define X32_display					peripherals[PERIPHERAL_DISPLAY]
#define X32_QR_timestamp 			peripherals[PERIPHERAL_XUFO_TIMESTAMP]
#define X32_rs232_data				peripherals[PERIPHERAL_PRIMARY_DATA]
#define X32_rs232_stat				peripherals[PERIPHERAL_PRIMARY_STATUS]
#define X32_rs232_char				(X32_rs232_stat & 0x02)
#define X32_wireless_data			peripherals[PERIPHERAL_WIRELESS_DATA]
#define X32_wireless_stat			peripherals[PERIPHERAL_WIRELESS_STATUS]
#define X32_wireless_char			(X32_wireless_stat & 0x02)
#define X32_button					peripherals[PERIPHERAL_BUTTONS]
#define X32_switches				peripherals[PERIPHERAL_SWITCHES]

 /*
 * more shorthands
 */
#define MINIMUM_ENGINE_SPEED 65
#define MAXIMUM_ENGINE_SPEED 1000
#define TRUE 1
#define FALSE 0
#define NULL 0
#define LOG_LENGTH 10000	//=10 seconds of logging in Manual Mode at 1000Hz
#define THRESHOLD 2000		//(ms) Communicaton safety mechanism threshold value
#define CALIBRATE_ITERATIONS 128 // Number of sensor readings needed for calibratio
#define TELEMETRY_FREQUENCY 250
#define CONTROL_LOOP_MIN_LIFT 4
//#define LOGGING


#define CONTROLS_THRESHOLD 130

#define SAFE_MODE_INT		0
#define PANIC_MODE_INT		1
#define MANUAL_MODE_INT		2
#define CALIBRATE_MODE_INT	3
#define YAW_CONTROL_INT		4
#define FULL_CONTROL_INT	5
#define P_VALUES_MODE_INT	6

#define FIFOSIZE 16


// Note: interrupts need to be disabled to safely perform this function
#define SET_ALL_ENGINE_RPM(rpm) ae[0]=ae[1]=ae[2]=ae[3]= rpm; 

#define PRINT_STATE(f) 	

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#endif
