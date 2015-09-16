#include "keyboardio.h"
#include "consoleio.h"
#include "input.h"

void processKeyboardEvent(char c, struct INPUT* keyboardInput) {
	bool updated;
	if ((c = term_getchar_nb()) != -1) {
		updated = true;
		switch(c) {
			// Controls
			case 'a':
				if (keyboardInput->lift < MAX_LEVEL) {	
					keyboardInput->lift++;
				}
				break;
			case 'z':
				if (keyboardInput->lift > -MAX_LEVEL) {
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
				if (keyboardInput->pitch > -MAX_LEVEL) {
					keyboardInput->pitch--;
				}
				break;
			case 66: // pitch down
				if (keyboardInput->pitch < MAX_LEVEL) {
					keyboardInput->pitch++;
				}
				break;
			case 68: // roll right
				if (keyboardInput->roll > -MAX_LEVEL) {
					keyboardInput->roll--;
				}
				break;
			case 67: // roll right
				if (keyboardInput->roll < MAX_LEVEL) {
					keyboardInput->roll++;
				}
				break;

			// Modes 
			case '0': // Safe mode
				printf("Set to safe mode (%x)\n", SAFE_MODE_INT);
				keyboardInput->mode = SAFE_MODE_INT;
				break;
			case '1': // Panic mode
				printf("Set to panic mode (%x)\n", PANIC_MODE_INT);
				keyboardInput->mode = PANIC_MODE_INT;
				break;
			case '2': // Manual mode
				printf("Set to manual mode (%x)\n", MANUAL_MODE_INT);
				keyboardInput->mode = MANUAL_MODE_INT;
				break;
			case '3': // Calibrate mode
				printf("Set to calibrate mode (%x)\n", CALIBRATE_MODE_INT);
				keyboardInput->mode = CALIBRATE_MODE_INT;
				break;
			case '4': // Yaw control mode
				printf("Set to 'Yaw control' mode (%x)\n", YAW_CONTROL_INT);
				keyboardInput->mode = YAW_CONTROL_INT;
				break;
			case '5': // Manual mode
				printf("Set to 'Full Control' mode (%x)\n", FULL_CONTROL_INT);
				keyboardInput->mode = FULL_CONTROL_INT;
				break;
			case 27: // Escape
				printf("Not implemented yet!\n");
				updated = false;
				break;

			// Controls for p values
			case 'u': // p control yaw
				printf("Not implemented yet!\n");
				updated = false;
				break;
			case 'j': // p control yaw
				printf("Not implemented yet!\n");
				updated = false;
				break;
			case 'i': // p1 control pitch/roll
				printf("Not implemented yet!\n");
				updated = false;
				break;
			case 'k': // p1 control pitch/roll
				printf("Not implemented yet!\n");
				updated = false;
				break;
			case 'o': // p2 control pitch/roll
				printf("Not implemented yet!\n");
				updated = false;
				break;
			case 'l': // p2 control pitch/roll
				printf("Not implemented yet!\n");
				updated = false;
				break;

			// Additional controls
			case '+': // Upload metrics
				keyboardInput->mode = 7;
				break;

			default:
				printf("Unknown key: %i\n", c);
				updated = false;
			// TODO: add other keys for P
		}
		keyboardInput->updated = updated;
	}
}