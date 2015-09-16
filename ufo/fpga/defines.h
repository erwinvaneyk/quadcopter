/*--------------------------
 define protocol specifics
 */

#define PACKET_SIZE 6
//HEADER
#define HEADER 0xAA //1010 1010

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