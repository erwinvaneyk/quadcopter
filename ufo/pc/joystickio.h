#ifndef _JOYSTICKIOH_
#define _JOYSTICKIOH_

#include "driver_joystick.h"
#include "input.h"
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
#define JS_AXIS_PITCH 		1

// roll: [-32767 = left, 32767 = right]
#define JS_AXIS_ROLL 		0

// yaw: [-32767 = counter-clockwise, 32767 = clockwise]
#define JS_AXIS_YAW			2

// throttle: [-32767 = down, 32767 = up]
#define JS_AXIS_LIFT	 	3


// Note that the numbers displayed on the JS are x + 1
#define JS_BUTTON_SAFE_MODE	 1

#define JS_BUTTON_PANIC_MODE 0

#define JS_BUTTON_MANUAL_MODE 2

#define JS_BUTTON_CALIBRATION_MODE 3

#define JS_BUTTON_YAW_CONTROL_MODE 4

#define JS_BUTTON_FULL_CONTROL_MODE 5

 // Buttons are numbered 0 - 11

short normalizeAxis(short axis_value, short buckets);
void processJoystickEvent(int fd, struct js_event js, struct JOYSTICK* joystick);
void updateJoystickInputModel(struct INPUT* joystickInputModel, struct JOYSTICK* joystick);
void show_joystick(struct JOYSTICK* joystick);

#endif