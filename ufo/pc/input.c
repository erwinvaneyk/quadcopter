#include "input.h"
#include <stdio.h>

int within_bounds(int x, int y) {
	if(x > y) {
		return y;
	}
	if(x < -y) {
		return -y;
	}
	return x;

}

// TODO: check validity of input-models
void updateInputModel(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick) {
	if (!keyboard->updated && !joystick->updated) 
		return;

	// Update controls 
	model->lift = within_bounds(keyboard->lift + joystick->lift, MAX_LEVEL);
	model->yaw = within_bounds(keyboard->yaw + joystick->yaw, MAX_LEVEL);
	model->roll = within_bounds(keyboard->roll + joystick->roll, MAX_LEVEL);
	model->pitch = within_bounds(keyboard->pitch + joystick->pitch, MAX_LEVEL);

	// Update mode (keyboard's mode overrides joystick's mode)
	if(keyboard->mode > -1) {
		model->mode = keyboard->mode;
	} else if(joystick->mode > -1) {
		model->mode = joystick->mode;
	}
	keyboard->mode = -1;
	joystick->mode = -1;

	// Update flags
	keyboard->updated = false;
	joystick->updated = false;
	model->updated = true;
}

void updateJoystickInputModel(struct INPUT* joystickInputModel, struct JOYSTICK* joystick) {
	if(!joystick->updated)
		return;

	// update controls
	joystickInputModel->pitch = normalizeAxis(joystick->axis[JS_AXIS_PITCH], NUMB_LEVELS) - MAX_LEVEL;
	joystickInputModel->yaw = normalizeAxis(joystick->axis[JS_AXIS_YAW], NUMB_LEVELS) - MAX_LEVEL;
	joystickInputModel->roll = normalizeAxis(joystick->axis[JS_AXIS_ROLL], NUMB_LEVELS) - MAX_LEVEL;
	joystickInputModel->lift = normalizeAxis(joystick->axis[JS_AXIS_LIFT], NUMB_LEVELS) - MAX_LEVEL;

	// Update mode
	int i;
	for(i = 0; i < MODES; i++) {
		if(joystick->button[i]) {
			joystickInputModel->mode = i;
			break;
		}
	}

	// Update flags
	joystick->updated = false;
	joystickInputModel->updated = true;
}

//DEBUG purpose
void show_input(struct INPUT* input) {
	printf("INPUT (%x) {\n", input->updated);
	printf("	lift: %x\n", input->lift);
	printf("	pitch: %x\n", input->pitch);
	printf("	roll: %x\n", input->roll);
	printf("	yaw: %x\n", input->yaw);
	printf("	mode: %x\n}\n", input->mode);
}