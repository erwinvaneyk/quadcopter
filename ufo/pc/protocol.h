/*------------------------------------------------------------
 * Communication PROTOCOL
 * 
 * <protocol.h>
 *------------------------------------------------------------
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/*--------------------------
 define protocol specifics
 */

#define PACKET_SIZE 6
//HEADER
#define HEADER 0xAA //1010 1010

//MODES
#define SAFE_MODE		0x00
#define PANIC_MODE		0xFF
#define MANUAL_MODE		0x01
#define CALIBRATE_MODE	0x02
#define YAW_CONTROL		0x03
#define FULL_CONTROL	0x04


//COMMANDS
#define LIFT	'L'  // <-- change this to codes, that way we can incorporate many at a time
#define ROLL	'R'
#define PITCH	'P'
#define YAW 	'Y'
#define P0 		'P'//control loop parameters
#define P1 		'P'//


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

#define NUM_LEVELS 	17

#include <stdint.h>

struct PACKET {
		uint8_t header;
		uint8_t mode;
		uint8_t command;
		uint16_t data;
		uint8_t checksum;
	} __attribute__ ((packed)) ;

//typedef struct PACKET packet;

void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data);
void show_pkt(struct PACKET* packet);
uint16_t level_convert(int level);

#endif
