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

// All rules regarding state/mode switches
void updateModeIfValid(struct INPUT* model, int newMode) {
	
	// Panic mode: Only a switch to safe mode is allowed
	if(model->mode == PANIC_MODE_INT && newMode != SAFE_MODE_INT) {
		return;
	}
	// Safe mode: All switches, except panic mode, should start at safe mode
	else if(model->mode != SAFE_MODE_INT model->mode = && newMode != PANIC_MODE_INT) {
		return;
	} 

	// Mode change is valid!
	model->mode = newMode;
}

void updateInputModel(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick) {
	if (!keyboard->updated && !joystick->updated) 
		return;


	// Update mode (keyboard's mode overrides joystick's mode)
	if(keyboard->mode != model->mode) {
		printf("Keyboard switched mode: %x\n", keyboard->mode);
		updateModeIfValid(model, keyboard->mode);
	} else if(joystick->mode != model->mode) {
		printf("Joystick switched mode: %x\n", joystick->mode);
		updateModeIfValid(model, joystick->mode);
	}
	// Sync modes across all input-devices
	keyboard->mode = joystick->mode = model->mode;

	// Update controls
	if (model->mode == SAFE_MODE_INT)
	{
		model->lift = keyboard->lift = joystick->lift 		= 0;
		model->yaw = keyboard->yaw = joystick->yaw 			= 0;
		model->roll = keyboard->roll = joystick->roll 		= 0;
		model->pitch = keyboard->pitch = joystick->pitch 	= 0;
	} 
	else if (model->mode == MANUAL_MODE_INT)
	{
		model->lift = within_bounds(keyboard->lift + joystick->lift, MAX_LEVEL);
		model->yaw = within_bounds(keyboard->yaw + joystick->yaw, MAX_LEVEL);
		model->roll = within_bounds(keyboard->roll + joystick->roll, MAX_LEVEL);
		model->pitch = within_bounds(keyboard->pitch + joystick->pitch, MAX_LEVEL);
	}

	// Update flags
	keyboard->updated = false;
	joystick->updated = false;
	model->updated = true;
}

//DEBUG purpose
void show_input(struct INPUT* input) {
	printf("INPUT (%i) {\n", input->updated);
	printf("	lift: %i\n", input->lift);
	printf("	pitch: %i\n", input->pitch);
	printf("	roll: %i\n", input->roll);
	printf("	yaw: %i\n", input->yaw);
	printf("	mode: %i\n}\n", input->mode);
}