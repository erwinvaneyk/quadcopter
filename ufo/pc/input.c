#include "input.h"
#include <stdio.h>
#include "tui.h"

//TUI related
extern int msg_cursor;

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
	
	// Safe mode: should be able to go to any mode
	if(model->mode == SAFE_MODE_INT) {
		model->mode = newMode;
		return;
	}

	// Panic mode: Only a switch to safe mode is allowed
	if(model->mode == PANIC_MODE_INT && newMode == SAFE_MODE_INT) {
		model->mode = newMode;
		return;
	}

	// Other modes: only allow going to panic mode or safe mode
	if(model->mode != SAFE_MODE_INT && model->mode != PANIC_MODE_INT && (newMode == SAFE_MODE_INT || newMode == PANIC_MODE_INT)) {
		model->mode = newMode;
		return;
	}
	
	//printf("Invalid mode transition %d -> %d\n", model->mode, newMode); 
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Invalid mode transition %d -> %d", model->mode, newMode);
	TUI_MOVE_CURSOR;

}

bool isSafeInputModel(struct INPUT* model) {
	return model->lift 	== 0 
		&& model->roll 	== 0
		&& model->pitch == 0
		&& model->yaw 	== 0;
}

void resetInputModel(struct INPUT* model) {
	model->lift		= 0;
	model->yaw		= 0;
	model->roll 	= 0;
	model->pitch 	= 0;
}

void updateInputModel(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick) {
	if (!keyboard->updated && !joystick->updated) {
		return;
	}

	// Do not allow control updates in safe mode
	if(model->mode == SAFE_MODE_INT && !(isSafeInputModel(joystick) && isSafeInputModel(model) && isSafeInputModel(keyboard))) {
		// Just to be sure, reset all models again
		resetInputModel(model);
		keyboard->mode = joystick->mode = model->mode = SAFE_MODE_INT;
		return;
	}

	// Update mode (keyboard's mode overrides joystick's mode)
	if(keyboard->mode != model->mode) {
		//printf("Keyboard switched mode: %x\n", keyboard->mode);
		mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Keyboard switched mode: %x", keyboard->mode);
		TUI_MOVE_CURSOR;
		updateModeIfValid(model, keyboard->mode);
	} else if(joystick->mode != model->mode) {
		//printf("Joystick switched mode: %x\n", joystick->mode);
		mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Joystick switched mode: %x", joystick->mode);
		TUI_MOVE_CURSOR;
		updateModeIfValid(model, joystick->mode);
	}
	// Sync modes across all input-devices
	keyboard->mode = joystick->mode = model->mode;

	// Update controls
	if (model->mode == SAFE_MODE_INT)
	{
		resetInputModel(model);
		resetInputModel(keyboard);
		resetInputModel(joystick);
	} 
	else if ( (model->mode == MANUAL_MODE_INT) || (model->mode == YAW_CONTROL_INT) )
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

//DEBUG purpose TODO: change to support TUI
void show_input(struct INPUT* input) {
	/*
	printf("INPUT (%i) {\n", input->updated);
	printf("	lift: %i\n", input->lift);
	printf("	pitch: %i\n", input->pitch);
	printf("	roll: %i\n", input->roll);
	printf("	yaw: %i\n", input->yaw);
	printf("	mode: %i\n}\n", input->mode);
	*/
	attron(COLOR_PAIR(6));
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input: INPUT (%i) {\n", input->updated);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input:   lift: %i\n", input->lift);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input:   pitch: %i\n", input->pitch);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input:   roll: %i\n", input->roll);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input:   yaw: %i\n", input->yaw);
	TUI_MOVE_CURSOR;
	mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "show_input:   mode: %i }\n", input->mode);
	TUI_MOVE_CURSOR;
	attroff(COLOR_PAIR(6));

}