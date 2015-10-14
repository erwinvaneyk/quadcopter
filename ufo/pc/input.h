#ifndef _INPUTH_
#define _INPUTH_

#include <stdbool.h>
#include <stdint.h>
/*
 * The input model, which is derived from the 
 * Lift, yaw, pitch, roll in levels
 */
struct INPUT {
	int lift;
	int yaw;
	int pitch;
	int roll;
	int mode;
	bool updated; // a simple flag to indicate whether the input has been updated
	// TODO: add control params and others when we have those implemented..
};

struct SPECIAL_INPUT {
	uint8_t yaw_p; //increment or decrement
	uint8_t current_yaw_p; //keep track of current P value //we dont need this really
	uint8_t full_p1;
	uint8_t full_p2;

	bool updated; // updated?
};

#define SAFE_MODE_INT		0
#define PANIC_MODE_INT		1
#define MANUAL_MODE_INT		2
#define CALIBRATE_MODE_INT	3
#define YAW_CONTROL_INT		4
#define FULL_CONTROL_INT	5
#define P_VALUES_MODE_INT	6

#define SEND_TELEMETRY_INT 7 //<----what is this?

#define NUMB_LEVELS 31
#define MAX_LEVEL 15
#define MODES 6 // counting from 0

void updateInputModel(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick);

void show_input(struct INPUT* input);

int within_bounds(int x, int lower_limit, int upper_limit);

#endif