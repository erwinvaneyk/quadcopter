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
