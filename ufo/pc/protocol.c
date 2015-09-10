/*------------------------------------------------------------
 * Communication protocol
 * 
 * <protocol.c>
 *------------------------------------------------------------
 */

 #include "protocol.h"
 #include <stdio.h>

	void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data)
	{
		packet->header 	= HEADER; 	// <--- hmm.....
		packet->mode 	= mode;
		packet->command = command;
		packet->data 	= data;
		packet->checksum = 0xFF;		// <--- hmm..... need to implement this
	};

	//DEBUG purpose
	void show_pkt(struct PACKET* packet)
	{
		printf("HEADER: %x\n", packet->header);
		printf("MODE: %x\n", packet->mode);
		printf("COMMAND: %x\n", packet->command);
		printf("DATA: %x\n", packet->data);
		printf("CHECKSUM: %x\n", packet->checksum);
	/*	int i;
		char *data = (char*)packet;
		for (i=0; i<sizeof(struct PACKET); i++)
		{
			printf("%02x ", data[i] & 0xff);
		}
	*/
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
			default :
				return HOVER; 
		}
	}