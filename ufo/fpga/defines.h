#define LOGGING
//#define DEBUG
#define TRUE 1
#define FALSE 0
#define LOG_LENGTH 10000	//=10 seconds of logging in Manual Mode at 1000Hz
#define THRESHOLD 2000		//(ms) Communicaton safety mechanism threshold value

/*
 * define some peripheral short hands
 */

#define X32_instruction_counter           peripherals[0x03]
#define X32_timer_per		peripherals[PERIPHERAL_TIMER1_PERIOD]
//#define X32_timer_per2		peripherals[PERIPHERAL_TIMER2_PERIOD]
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


/*
 * define protocol specifics
 */

#define PACKET_SIZE 7
//HEADER
#define HEADER 0xAA
//MODES or COMMANDS
#define COMMAND_MODE	0x00
#define SAFE_MODE		0x01
#define MANUAL_MODE		0x02
#define CALIBRATE_MODE	0x03
#define YAW_CONTROL		0x04
#define FULL_CONTROL	0x05
#define PANIC_MODE		0x0F

#define SEND_TELEMETRY	0x07


//DATA
#define HOVER 	0x0000 //level zero
#define LEVEL1 	0x0001
#define LEVEL2 	0x0002
#define LEVEL3 	0x0003
#define LEVEL4 	0x0004
#define LEVEL5 	0x0005
#define LEVEL6 	0x0006
#define LEVEL7 	0x0007
#define LEVEL8 	0x0008
#define LEVEL9 	0x0009
#define LEVEL10 	0x000A
#define LEVEL11 	0x000B
#define LEVEL12 	0x000C
#define LEVEL13 	0x000D
#define LEVEL14 	0x000E
#define LEVEL15 	0x000F

//1 bit to indicate negative value
#define LEVEL_N1 	0x0011
#define LEVEL_N2 	0x0012
#define LEVEL_N3 	0x0013
#define LEVEL_N4 	0x0014
#define LEVEL_N5 	0x0015
#define LEVEL_N6 	0x0016
#define LEVEL_N7 	0x0017
#define LEVEL_N8 	0x0018
#define LEVEL_N9 	0x0019
#define LEVEL_N10 	0x001A
#define LEVEL_N11 	0x001B
#define LEVEL_N12 	0x001C
#define LEVEL_N13 	0x001D
#define LEVEL_N14 	0x001E
#define LEVEL_N15 	0x001F


 /*
 * more shorthands
 */

#define MINIMUM_ENGINE_SPEED 65

#define SAFE_MODE_INT		0
#define PANIC_MODE_INT		1
#define MANUAL_MODE_INT		2
#define CALIBRATE_MODE_INT	3
#define YAW_CONTROL_INT		4
#define FULL_CONTROL_INT	5

#define FIFOSIZE 16

#define PANIC_AND_EXIT 	modecommand = PANIC_MODE;\
						process_packet();\
						ALIVE = 0;

#define SET_ALL_ENGINE_RPM(rpm) ae[0]=ae[1]=ae[2]=ae[3]= rpm;

#define PRINT_STATE(f) 	if (count%f == 0)\
			{\
				print_state();\
				count = 1;\
			}\
		else count++;
