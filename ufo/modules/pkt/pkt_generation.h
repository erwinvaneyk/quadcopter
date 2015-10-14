/*------------------------------------------------------------
 * Communication PROTOCOL
 * 
 * <protocol.h>
 *------------------------------------------------------------
 */
#ifndef PKT_GENERATION_H_
#define PKT_GENERATION_H_

#include <sys/time.h>
#include <stdint.h>
#include "pkt.h"
#include "../../pc/input.h"

struct PACKET {
	uint8_t header;
	uint8_t modecommand;
	uint32_t data;
	uint8_t checksum;
} __attribute__ ((packed)) ;

typedef struct PACKET pkt;

void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data);
void input_to_pkt(struct INPUT* inputModel, struct PACKET* packet, struct SPECIAL_INPUT* p_input);
void show_pkt(struct PACKET* packet);
#endif
