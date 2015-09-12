/*------------------------------------------------------------
 * Communication protocol
 * 
 * <protocol.c>
 *------------------------------------------------------------
 */

 #include "protocol.h"
 #include <stdio.h>

	void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data) {
		packet->header 	= HEADER;
		packet->mode 	= mode;
		packet->command = command;
		packet->data 	= data;
		packet->checksum = mode ^ command ^ (data&0x00FF) ^ ( data>>8 &0x00FF );
	};

	//DEBUG purpose
	void show_pkt(struct PACKET* packet)
	{
		printf("HEADER: %x\n", packet->header);
		printf("MODE: %x\n", packet->mode);
		printf("COMMAND: %x\n", packet->command);
		printf("DATA: %x\n", packet->data);
		printf("CHECKSUM: %x\n", packet->checksum);
		printf("\n");
	}

	uint16_t level_convert(int level)
	{
		switch(level){
			case 0:
				return HOVER;
			case 1:
				return LEVEL1;
			case 2:
				return LEVEL2;
			case 3:
				return LEVEL3;
			case 4:
				return LEVEL4;
			case 5:
				return LEVEL5;
			case 6:
				return LEVEL6;
			case 7:
				return LEVEL7;
			case 8:
				return LEVEL8;

			case 9:
				return LEVEL9;
			case 10:
				return LEVEL10;
			case 11:
				return LEVEL11;
			case 12:
				return LEVEL12;
			case 13:
				return LEVEL13;
			case 14:
				return LEVEL14;
			case 15:
				return LEVEL15;

			case -1:
				return LEVEL_N1;
			case -2:
				return LEVEL_N2;
			case -3:
				return LEVEL_N3;
			case -4:
				return LEVEL_N4;
			case -5:
				return LEVEL_N5;
			case -6:
				return LEVEL_N6;
			case -7:
				return LEVEL_N7;
			case -8:
				return LEVEL_N8;

			case -9:
				return LEVEL_N9;
			case -10:
				return LEVEL_N10;
			case -11:
				return LEVEL_N11;
			case -12:
				return LEVEL_N12;
			case -13:
				return LEVEL_N13;
			case -14:
				return LEVEL_N14;
			case -15:
				return LEVEL_N15;
			default :
				return HOVER; 
		}
	}