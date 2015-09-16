/*------------------------------------------------------------
 * Communication PROTOCOL
 * 
 * <protocol.h>
 *------------------------------------------------------------
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "input.h"
/*--------------------------
 define protocol specifics
 */

#define PACKET_SIZE 7
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

#define ACK				0xF0

#define SEND_TELEMETRY	0x07


//DATA
#define HOVER 	0x00 //level zero
#define LEVEL1 	0x01
#define LEVEL2 	0x02
#define LEVEL3 	0x03
#define LEVEL4 	0x04
#define LEVEL5 	0x05
#define LEVEL6 	0x06
#define LEVEL7 	0x07
#define LEVEL8 	0x08
 #define LEVEL9 	0x09
 #define LEVEL10 	0x0A
 #define LEVEL11 	0x0B
 #define LEVEL12 	0x0C
 #define LEVEL13 	0x0D
 #define LEVEL14 	0x0E
 #define LEVEL15 	0x0F

//1 bit to indicate negative value
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

#include <stdint.h>

struct PACKET {
		uint8_t header;
		uint8_t modecommand;
		uint32_t data;
		uint8_t checksum;
	} __attribute__ ((packed)) ;

//typedef struct PACKET packet;

void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data);
void input_to_pkt(struct INPUT* inputModel, struct PACKET* packet);
void show_pkt(struct PACKET* packet);
uint8_t level_convert(int level);

#endif
