/*
* Terminal User Interface Useful Code
* 
* int msg_cursor is the current cursor for the message field
* globally declared in 
*/
#ifndef TUI_H
#define TUI_H

#include <ncurses.h>

extern int msg_cursor;

#define MESSAGE_FIELD_START 8
#define MESSAGE_FIELD_SIZE 20
#define MODE_LINE 30
#define BASE_Y 15
#define BASE_X 65

#define CURRENT_MSG_CURSOR MESSAGE_FIELD_START + msg_cursor

#define STATUS 	1
#define MESSAGE 2

#define TUI_INIT_COLORS \
	init_pair(1, COLOR_BLACK, COLOR_GREEN);\
	init_pair(2, COLOR_RED, COLOR_BLACK);\
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);\
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);\
	init_pair(5, COLOR_BLACK, COLOR_YELLOW);\
	init_pair(6, COLOR_MAGENTA, COLOR_GREEN);\

#define TUI_MOVE_CURSOR \
	msg_cursor++;\
	if (msg_cursor > MESSAGE_FIELD_SIZE)\
	{\
		while(msg_cursor != 0)\
		{\
			move(msg_cursor+MESSAGE_FIELD_START,0);\
			clrtoeol();\
			msg_cursor--;\
		}\
	}\


#define TUI_PRINT_MESSAGE(msg) \
	mvprintw(MESSAGE_FIELD_START + msg_cursor,0,#msg);\
	TUI_MOVE_CURSOR;\

#define TUI_PRINT_MODE(msg)\
	move(MODE_LINE,0);\
	clrtoeol();\
	attron(COLOR_PAIR(5));\
	printw(#msg);\
	attroff(COLOR_PAIR(5));\


void TUI_engines(int ae0, int ae1, int ae2, int ae3);
void TUI_engines_init(void);


#endif

//Alternatively:
//mvprintw(MESSAGE_FIELD_START + msg_cursor,0,"Set to safe mode (%d)\n", SAFE_MODE_INT);
