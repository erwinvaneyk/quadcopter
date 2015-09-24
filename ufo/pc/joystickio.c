#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "joystickio.h"
#include "protocol.h"

short normalizeAxis(short axis_value, short buckets) {
	assert(buckets > 0);
	short normalizedValue = (axis_value / 2) + ((SHRT_MAX / 2 + 1)); 
	short bucket_size = SHRT_MAX / buckets + 1;
	if(bucket_size < 0) {
		bucket_size = SHRT_MAX;
	}

	short i;
	for(i = 0; i < buckets; i += 1) {
		if(normalizedValue >= (i * bucket_size) && normalizedValue <= ((i + 1) * bucket_size)) {
			return i;
		}
	}
	return -1;
}

void processJoystickEvent(int fd, struct js_event js, struct JOYSTICK* joystick) {
	fcntl(fd, F_SETFL, O_NONBLOCK); // SHould be set in some init

	if(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {
		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				joystick->button[js.number] = js.value > 0;
				break;
			case JS_EVENT_AXIS:
				joystick->axis[js.number] = -js.value;
				break;
		}
		joystick->updated = true;
	}
}


void updateJoystickInputModel(struct INPUT* joystickInputModel, struct JOYSTICK* joystick) {
	if(!joystick->updated)
		return;

	// Update mode
	int i;
	for(i = 0; i < MODES; i++) {
		if(joystick->button[i]) {
			joystickInputModel->mode = i;
			break;
		}
	}

	// Ignore any updates when safe mode is
	if(joystickInputModel->mode != SAFE_MODE_INT && joystickInputModel->mode != PANIC_MODE_INT) {
		// update controls
		joystickInputModel->pitch = normalizeAxis(joystick->axis[JS_AXIS_PITCH], NUMB_LEVELS) - MAX_LEVEL;
		joystickInputModel->yaw = normalizeAxis(joystick->axis[JS_AXIS_YAW], NUMB_LEVELS) - MAX_LEVEL;
		joystickInputModel->roll = normalizeAxis(joystick->axis[JS_AXIS_ROLL], NUMB_LEVELS) - MAX_LEVEL;
		joystickInputModel->lift = (normalizeAxis(joystick->axis[JS_AXIS_LIFT], NUMB_LEVELS) + 1) / 2 ;
	}

	// Update flags
	joystick->updated = false;
	joystickInputModel->updated = true;
}

//DEBUG purpose
void show_joystick(struct JOYSTICK* joystick){
	printf("JOYSTICK: {\n");
	printf("	PITCH: %x\n", joystick->axis[JS_AXIS_PITCH]);
	printf("	ROLL: %x\n", joystick->axis[JS_AXIS_ROLL]);
	printf("	LIFT: %x\n", joystick->axis[JS_AXIS_LIFT]);
	printf("	YAW: %x\n}\n", joystick->axis[JS_AXIS_YAW]);
}