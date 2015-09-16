#include "keyboardio.h"
#include "consoleio.h"
#include "input.h"

void processKeyboardEvent(char c, struct INPUT* keyboardInput) {
	if ((c = term_getchar_nb()) != -1) {
		switch(c) {
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
			case 'u':
				keyboardInput->mode = 7;
				break;
			// TODO: add other keys
		}
		keyboardInput->updated = true;
	}
}