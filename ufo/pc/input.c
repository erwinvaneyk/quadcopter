#include "input.h"
#include "joystickio.h"
#define WITHIN_BOUNDS(X, Y) (((X) < (Y) && (X) > -(Y)) ? (X) : (Y))

void updateInputModel(struct *INPUT model, struct *INPUT keyboard. struct *INPUT joystick) {
	if (!keyboard->updated && !joystick->updated) 
		return;

	// Update controls 
	model->lift = WITHIN_BOUNDS(keyboard->lift + joystick->lift, MAX_LEVEL);
	model->yaw = WITHIN_BOUNDS(keyboard->yaw + joystick->yaw, MAX_LEVEL);
	model->roll = WITHIN_BOUNDS(keyboard->roll + joystick->roll, MAX_LEVEL);
	model->pitch = WITHIN_BOUNDS(keyboard->pitch + joystick->pitch, MAX_LEVEL);

	// Update mode (keyboard's mode overrides joystick's mode)
	if(keyboard->mode > -1) {
		model->mode = keyboard->mode;
	} else if(joystick.mode > -1) {
		model->mode = joystick->mode;
	}
	keyboard->mode = -1;
	joystick->mode = -1;

	// Update flags
	keyboard->updated = false;
	joystick->updated = false;
	model->updated = true;
}

void processJoystickInput(struct *INPUT joystickInputModel, struct *JOYSTICK joystick) {
	if(!joystick.updated)
		return;

	// update controls
	joystick->pitch = normalizeAxis(joystick->axis[JS_AXIS_PITCH], NUMB_LEVELS) - MAX_LEVEL;
	joystick->yaw = normalizeAxis(joystick->axis[JS_AXIS_YAW], NUMB_LEVELS) - MAX_LEVEL;
	joystick->roll = normalizeAxis(joystick->axis[JS_AXIS_ROLL], NUMB_LEVELS) - MAX_LEVEL;
	joystick->lift = normalizeAxis(joystick->axis[JS_AXIS_LIFT], NUMB_LEVELS) - MAX_LEVEL;

	// Update mode
	int i;
	for(i = 0; i < MODES; i++) {
		if(joystick->button[i]) {
			joystickInputModel->mode = i;
			updated = true;
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