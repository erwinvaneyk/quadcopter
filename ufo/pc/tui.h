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
#define MESSAGE_FIELD_SIZE 15
#define MODE_LINE 25

#define CURRENT_MSG_CURSOR MESSAGE_FIELD_START + msg_cursor

#define STATUS 	1
#define MESSAGE 2

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





#endif

//void TUI_MODE(struct INPUT* model)
//{
	//mvprintw(MESSAGE_FIELD_START + msg_cursor,0,#msg);
//}

//Alternatively:
//mvprintw(MESSAGE_FIELD_START + msg_cursor,0,"Set to safe mode (%d)\n", SAFE_MODE_INT);
