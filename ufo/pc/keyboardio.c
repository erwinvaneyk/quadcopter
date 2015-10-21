#include "keyboardio.h"
#include "consoleio.h"
#include "input.h"
#include "tui.h"

//TUI related
extern int msg_cursor;
int i;

void processKeyboardEvent(char c, struct INPUT* keyboardInput, struct SPECIAL_INPUT* p_input) {
	bool updated;
	if ((c = term_getchar_nb()) != -1) {
		updated = true;
		bool unknownKey = true;
		if(keyboardInput->mode != SAFE_MODE_INT && keyboardInput->mode != PANIC_MODE_INT) {
			unknownKey = false;
			switch(c) {
				// Controls
				case 'a':
					if (keyboardInput->lift < MAX_LEVEL) {	
						keyboardInput->lift++;
					}
					break;
				case 'z':
					if (keyboardInput->lift > 0) {
						keyboardInput->lift--;
					}
					break;

				case 'q': // yaw left
					if (keyboardInput->yaw > -MAX_LEVEL) {
						keyboardInput->yaw--;
					}
					break;
				case 'w': // yaw right
					if (keyboardInput->yaw < MAX_LEVEL) {
						keyboardInput->yaw++;
					}
					break;
				case 65: // pitch up
					if (keyboardInput->pitch < MAX_LEVEL) {
						keyboardInput->pitch++;
					}
					break;
				case 66: // pitch down
					if (keyboardInput->pitch > -MAX_LEVEL) {
						keyboardInput->pitch--;
					}
					break;
				case 68: // roll right
					if (keyboardInput->roll > -MAX_LEVEL) {
						keyboardInput->roll++;
					}
					break;
				case 67: // roll right
					if (keyboardInput->roll < MAX_LEVEL) {
						keyboardInput->roll--;
					}
					break;
				default:
					unknownKey = true;
					break;
			}
		}	

		switch(c) {
			// Modes 
			case '0': // Safe mode
				TUI_PRINT_MESSAGE(Set to safe mode);
				keyboardInput->mode = SAFE_MODE_INT;
				break;
			case '1': // Panic mode
				TUI_PRINT_MESSAGE(Set to panic mode);
				keyboardInput->mode = PANIC_MODE_INT;
				break;
			case '2': // Manual mode
				TUI_PRINT_MESSAGE(Set to manual mode);
				keyboardInput->mode = MANUAL_MODE_INT;
				break;
			case '3': // Calibrate mode
				TUI_PRINT_MESSAGE(Set to calibrate mode);
				keyboardInput->mode = CALIBRATE_MODE_INT;
				break;
			case '4': // Yaw control mode
				TUI_PRINT_MESSAGE(Set to 'Yaw control' mode);
				keyboardInput->mode = YAW_CONTROL_INT;
				break;
			case '5': // Manual mode
				TUI_PRINT_MESSAGE(Set to 'Full Control' mode);
				keyboardInput->mode = FULL_CONTROL_INT;
				break;
			case 27: // Escape
				TUI_PRINT_MESSAGE(Set to panic mode);
				keyboardInput->mode = PANIC_MODE_INT;
				updated = false;
				break;

			// Controls for p values
			case 'u': // p control yaw
				TUI_PRINT_MESSAGE(Incremented P value for YAW!);
				p_input->yaw_p = within_bounds(p_input->yaw_p+1,1,20);
				p_input->updated = true;
				/*#define DEBUG
				#ifdef DEBUG
				attron(COLOR_PAIR(6));
				mvprintw(MESSAGE_FIELD_START + msg_cursor,0,"yaw_p++: yaw_p = %d", p_input->yaw_p);
				attroff(COLOR_PAIR(6));
				#endif*/
				break;
			case 'j': // p control yaw
				//printf("Decremented P value for YAW!\n");
				TUI_PRINT_MESSAGE(Decremented P value for YAW!);
				p_input->yaw_p = within_bounds(p_input->yaw_p-1,1,20);
				p_input->updated = true;
				break;
			case 'i': // p1 control pitch/roll
				TUI_PRINT_MESSAGE(Incremented P1 value for FULL CONTROL!);
				p_input->full_p1 = within_bounds(p_input->full_p1+1,1,20);
				p_input->updated = true;
				break;
			case 'k': // p1 control pitch/roll
				TUI_PRINT_MESSAGE(Decremented P1 value for FULL CONTROL!);
				p_input->full_p1 = within_bounds(p_input->full_p1-1,1,20);
				p_input->updated = true;
				break;
			case 'o': // p2 control pitch/roll
				TUI_PRINT_MESSAGE(Incremented P2 value for FULL CONTROL!);
				p_input->full_p2 = within_bounds(p_input->full_p2+1,1,20);
				p_input->updated = true;
				break;
			case 'l': // p2 control pitch/roll
				TUI_PRINT_MESSAGE(Decremented P2 value for FULL CONTROL!);
				p_input->full_p2 = within_bounds(p_input->full_p2-1,1,20);
				p_input->updated = true;
				break;

			// Additional controls
			case '+': // Upload metrics
				//printf("Set to 'LOGGING' mode (%x)\n", SEND_TELEMETRY_INT);
				TUI_PRINT_MESSAGE(Set to 'LOGGING' mode);
				keyboardInput->mode = SEND_TELEMETRY_INT;
				break;

			case 'p': // p2 control pitch/roll
				TUI_PRINT_MESSAGE(Increased Sensitivity);
				p_input->sensitivity = within_bounds(p_input->sensitivity+1,0,60); //needs more modifications for negative values, hence from 0 to 60
				p_input->updated = true;
				break;

			case ';': // p2 control pitch/roll
				TUI_PRINT_MESSAGE(Decreased Sensitivity);
				p_input->sensitivity = within_bounds(p_input->sensitivity-1,0,60);
				p_input->updated = true;
				break;

			case 'c': // clean messages
				for (i = MESSAGE_FIELD_START; i <= (MESSAGE_FIELD_START + MESSAGE_FIELD_SIZE); i++)
				{
					move(i, 0);  // move to begining of line
					clrtoeol();  // clear line
				}
				move(MESSAGE_FIELD_START, 0); 
				msg_cursor = 0;
				break;

			default:
				if(unknownKey) {
					#ifdef DEBUG_KEYBOARD
						attron(COLOR_PAIR(6));
						mvprintw(MESSAGE_FIELD_START + msg_cursor,0,"Unknown key: %i\n", c);
						attroff(COLOR_PAIR(6));
						TUI_MOVE_CURSOR;
					#endif
					updated = false;
				}
		}
		keyboardInput->updated = updated;
	}
}
