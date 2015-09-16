#include "protocol.h"
#include "joystickio.h"
#include <stdbool.h>

/*
 * The input model, which is derived from the 
 * Lift, yaw, pitch, roll in levels
 */
struct INPUT {
	short lift;
	short yaw;
	short pitch;
	short roll;
	short mode;
	bool updated; // a simple flag to indicate whether the input has been updated
	// TODO: add control params and others when we have those implemented..
};

#define NUMB_LEVELS 31;
#define MAX_LEVEL 15;
#define MODES 6; // counting from 0

void updateInputModel(struct *INPUT model, struct *INPUT keyboard. struct *INPUT joystick);

void processJoystickInput(struct *INPUT joystickInputModel, struct *JOYSTICK joystick);

void show_input(struct INPUT* input);