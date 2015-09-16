#ifndef _JOYSTICKIOH_
#define _JOYSTICKIOH_

#include "driver_joystick.h"
#include <stdbool.h>

/*
 * Definitions for the output of the joystick
 */

struct JOYSTICK {
	short	axis[6];
	bool	button[12];
	bool	updated;
};

// Axis
// pitch: [-32767 = front, 32767 = back]
#define JS_AXIS_PITCH 		0

// roll: [-32767 = left, 32767 = right]
#define JS_AXIS_ROLL 		1

// yaw: [-32767 = counter-clockwise, 32767 = clockwise]
#define JS_AXIS_YAW			2

// throttle: [-32767 = down, 32767 = up]
#define JS_AXIS_LIFT	 	3

 // Buttons are numbered 0 - 11

short normalizeAxis(short axis_value, short buckets);
void processJoystickEvent(int fd, struct js_event js, struct JOYSTICK* joystick);
void show_joystick(struct JOYSTICK* joystick);

#endif