/*------------------------------------------------------------
 * Communication PROTOCOL
 * 
 * <pkt.h>
 *------------------------------------------------------------
 */
#ifndef PKT_PROTOCOL_H_
#define PKT_PROTOCOL_H_

// Protocol specific constants
#define COMM_T 40000 //value in us 25 Hz
#define PACKET_SIZE 7
#define HEADER 0xAA //1010 1010

// Default modes
#define COMMAND_MODE	0x00
#define SAFE_MODE		0x01
#define MANUAL_MODE		0x02
#define CALIBRATE_MODE	0x03
#define YAW_CONTROL		0x04
#define FULL_CONTROL	0x05
#define PANIC_MODE		0x0F
#define P_VALUES_MODE	0x42

// Additional special modes/commands
#define SEND_TELEMETRY	0x07

// DATA
#define LEVEL0 		0x00
#define LEVEL1 		0x01
#define LEVEL2 		0x02
#define LEVEL3 		0x03
#define LEVEL4 		0x04
#define LEVEL5 		0x05
#define LEVEL6 		0x06
#define LEVEL7 		0x07
#define LEVEL8 		0x08
#define LEVEL9 		0x09
#define LEVEL10 	0x0A
#define LEVEL11 	0x0B
#define LEVEL12 	0x0C
#define LEVEL13 	0x0D
#define LEVEL14 	0x0E
#define LEVEL15 	0x0F

// Note: 1 bit to indicate negative value
#define LEVEL_N1 	0x11
#define LEVEL_N2 	0x12
#define LEVEL_N3 	0x13
#define LEVEL_N4 	0x14
#define LEVEL_N5 	0x15
#define LEVEL_N6 	0x16
#define LEVEL_N7 	0x17
#define LEVEL_N8 	0x18
#define LEVEL_N9 	0x19
#define LEVEL_N10 	0x1A
#define LEVEL_N11 	0x1B
#define LEVEL_N12 	0x1C
#define LEVEL_N13 	0x1D
#define LEVEL_N14 	0x1E
#define LEVEL_N15 	0x1F

#define NUM_LEVELS 	31

#endif
