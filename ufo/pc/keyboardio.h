#ifndef _KEYBOARDIOH_
#define _KEYBOARDIOH_

/*
 * Provides interface for mapping the keyboard to the INPUT-model and the SPECIAL_INPUT-model.
 */

#include "input.h"
#include <stdio.h>

void processKeyboardEvent(char c, struct INPUT* keyboardInput, struct SPECIAL_INPUT* p_input);

#endif