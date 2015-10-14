/*------------------------------------------------------------
 * Communication protocol
 * 
 * <protocol.c>
 *------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "pkt_generation.h"
#include "pkt.h"

#include "../../pc/tui.h"

uint8_t level_convert(int level);
uint8_t convert_modecommand(int mode);

void add_checksum(struct PACKET* packet) {
	packet->checksum = packet->modecommand 
		^ (packet->data & 0x000000FF) 
		^ (packet->data >> 8 & 0x000000FF) 
		^ (packet->data >> 16 & 0x000000FF) 
		^ (packet->data >> 24 & 0x000000FF);
}

void generate_pkt(struct PACKET* packet, uint8_t mode, uint8_t command, uint16_t data) {
	packet->header 	= HEADER;
	packet->modecommand = mode;
	packet->data 	= data;
	add_checksum(packet);
};


uint32_t convert_data(struct INPUT* inputModel) {
	uint32_t data;

	data = level_convert(inputModel->roll) << 8;
	data = (data | level_convert(inputModel->pitch)) << 8;
	data = (data | level_convert(inputModel->yaw)) << 8;
	data = data | level_convert(inputModel->lift);

	return data;
}

uint32_t convert_p_values_to_data(struct SPECIAL_INPUT* p_input)
{
	uint32_t data;

	data = 0x00 << 8; //data4
	data = (data | p_input->full_p2) << 8; //data3 Full-P2
	data = (data | p_input->full_p1) << 8; //data2 Full-P1
	data = data | p_input->yaw_p; //data1 Yaw-P

	return data;
}

void input_to_pkt(struct INPUT* inputModel, struct PACKET* packet, struct SPECIAL_INPUT* p_input) {
	packet->header = HEADER;

	switch(p_input->updated) {
		case true:
			packet->modecommand = P_VALUES_MODE;
			packet->data = convert_p_values_to_data(p_input);
			p_input->updated = false;
			break;
		case false:
			packet->modecommand = convert_modecommand(inputModel->mode);
			packet->data = convert_data(inputModel);
			break;
	}
	
	add_checksum(packet);
}

uint8_t level_convert(int level)
{
	switch(level) {
		case 0:
			return LEVEL0;
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
			return LEVEL0; 
	}
}

uint8_t convert_modecommand(int mode) {
	switch(mode) {
		case SAFE_MODE_INT:
			return SAFE_MODE;
		case PANIC_MODE_INT:
			return PANIC_MODE;
		case MANUAL_MODE_INT:
			return MANUAL_MODE;
		case CALIBRATE_MODE_INT:
			return CALIBRATE_MODE;
		case YAW_CONTROL_INT:
			return YAW_CONTROL;
		case FULL_CONTROL_INT:
			return FULL_CONTROL;
		case SEND_TELEMETRY_INT:
			return SEND_TELEMETRY;
		default:
			return SAFE_MODE;
	}
}

//DEBUG purpose
void show_pkt(struct PACKET* packet)
{
	attron(COLOR_PAIR(6));
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_pkt: PACKET: {\n");
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_pkt:   HEADER: %x\n", packet->header);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_pkt:   MODECOMMAND: %x\n", packet->modecommand);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_pkt:   DATA: %x\n", packet->data);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_pkt:   CHECKSUM: %x }\n", packet->checksum);
	TUI_MOVE_CURSOR;
	attroff(COLOR_PAIR(6));
}
