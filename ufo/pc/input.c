#include "input.h"
#include <stdio.h>
#include "tui.h"

//TUI related
extern int msg_cursor;

int within_bounds(int x, int lower_limit, int upper_limit) {
	if(x > upper_limit) {
		return upper_limit;
	}
	if(x < lower_limit) {
		return lower_limit;
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

//re1all controls besides the lift; for Threshold purpose
void resetAllModels(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick) {

	model->yaw			= 0;
	model->roll			= 0;
	model->pitch		= 0;

	keyboard->yaw		= 0;
	keyboard->roll		= 0;
	keyboard->pitch		= 0;

	joystick->yaw		= 0;
	joystick->roll		= 0;
	joystick->pitch		= 0;
}

void updateInputModel(struct INPUT* model, struct INPUT* keyboard, struct INPUT* joystick) {
	if (!keyboard->updated && !joystick->updated) {
		return;
	}

	// Do not allow control updates in safe mode
	if(model->mode == SAFE_MODE_INT && !(isSafeInputModel(joystick) && isSafeInputModel(model) && isSafeInputModel(keyboard))) {
		// Just to be sure, reset all models again
		if(!isSafeInputModel(joystick)) {
			mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Joystick unsafe!");
			TUI_MOVE_CURSOR;
		}
		if(!isSafeInputModel(keyboard)) {
			mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Keyboard unsafe!");
			TUI_MOVE_CURSOR;
		}
		resetInputModel(model);
		keyboard->mode = joystick->mode = model->mode = SAFE_MODE_INT;
		return;
	}

	// Update mode (keyboard's mode overrides joystick's mode)
	if(keyboard->mode != model->mode) {
		mvprintw(MESSAGE_FIELD_START + msg_cursor, 0, "Keyboard switched mode: %x", keyboard->mode);
		TUI_MOVE_CURSOR;
		updateModeIfValid(model, keyboard->mode);
	} else if(joystick->mode != model->mode) {
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
	else if ( (model->mode == MANUAL_MODE_INT) || (model->mode == YAW_CONTROL_INT) || (model->mode == FULL_CONTROL_INT)  )
	{
		model->lift = within_bounds(keyboard->lift + joystick->lift,  0, MAX_LEVEL);
		if (model->lift >=  CONTROLS_THRESHOLD) //disable other controls until rpm=130 is achieved
		{
			model->yaw = within_bounds(keyboard->yaw + joystick->yaw, -MAX_LEVEL, MAX_LEVEL);
			model->roll = within_bounds(keyboard->roll + joystick->roll, -MAX_LEVEL, MAX_LEVEL);
			model->pitch = within_bounds(keyboard->pitch + joystick->pitch, -MAX_LEVEL, MAX_LEVEL);
		}
		else 
		{
			resetAllModels(model,keyboard,joystick); //resets all but lift
		}
	}

	// Update flags
	keyboard->updated = false;
	joystick->updated = false;
	model->updated = true;
}

//DEBUG purpose TODO: change to support TUI
void show_input(struct INPUT* input) {
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