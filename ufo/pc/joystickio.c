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
	if(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {	
		switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				joystick->button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				joystick->axis[js.number] = js.value > 0;
				break;
		}
		joystick->updated = true;
	}
}


//DEBUG purpose
void show_joystick(struct JOYSTICK* joystick){
	printf("PITCH: %x\n", joystick->axis[JS_AXIS_PITCH]);
	printf("ROLL: %x\n", joystick->axis[JS_AXIS_ROLL]);
	printf("THROTTLE: %x\n", joystick->axis[JS_AXIS_THROTTLE]);
	printf("YAW: %x\n", joystick->axis[JS_AXIS_YAW]);
}