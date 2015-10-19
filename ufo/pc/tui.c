#include "tui.h"

void TUI_frame_init(void)
{
	start_color();
	TUI_INIT_COLORS;

	attron(A_BOLD | A_STANDOUT ); 
	attron(COLOR_PAIR(1));
	mvprintw(0,0, "- - - - - - - - - - - - - - -  Quadcopter Terminal  - - - - - - - - - - - - - - ");
	mvprintw(1,0, "                                                                                ");
	mvprintw(2,0, "STATUS:                                                                         ");
	attroff(COLOR_PAIR(1));

	attron(COLOR_PAIR(2));
	attron(A_UNDERLINE);
	mvprintw(3,0, "***ENGINE RPM***|   ***SENSOR values***   | ***CONTROL values***    ");
	attroff(A_UNDERLINE);
	mvprintw(4,0, "AE0 AE1 AE2 AE3 |AX   AY   AZ   P   Q   R |YP:P1:P2:S               ");
	attroff(COLOR_PAIR(2));
	attroff(A_BOLD | A_STANDOUT );

	attron(A_BOLD | A_STANDOUT );
	attron(COLOR_PAIR(1));
	mvprintw(6,0, "                                                                                ");
	attroff(COLOR_PAIR(1));

	attron(A_BOLD | A_STANDOUT ); 
	attron(COLOR_PAIR(1));
	mvprintw(7,0, "MESSAGES:                                                                       ");
	attroff(COLOR_PAIR(1));
	attroff(A_BOLD | A_STANDOUT ); 
}

void TUI_engines_init(void)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		mvprintw(BASE_Y+i, BASE_X-1, "|   | |   | |   | |   |");
	}
	mvprintw(BASE_Y+i, BASE_X-1, "|___| |___| |___| |___|");
	mvprintw(BASE_Y-1, BASE_X-1, " ___   ___   ___   ___ ");
	mvprintw(BASE_Y+11, BASE_X, "AE0");
	mvprintw(BASE_Y+11, BASE_X+6, "AE1");
	mvprintw(BASE_Y+11, BASE_X+12, "AE2");
	mvprintw(BASE_Y+11, BASE_X+18, "AE3");

	init_pair(10, COLOR_BLACK, COLOR_GREEN);
	init_pair(11, COLOR_BLACK, COLOR_YELLOW);
	init_pair(12, COLOR_BLACK, COLOR_RED);

}


#define ELSE(offset) \
			{\
				attron(COLOR_PAIR(2));\
				mvprintw(BASE_Y+10-i, BASE_X+offset, "   ");\
			}\

void TUI_engines(int ae0, int ae1, int ae2, int ae3)
{
	int i;
	int ratio0 = ae0/100;
	int ratio1 = ae1/100;
	int ratio2 = ae2/100;
	int ratio3 = ae3/100;

	for (i = 0; i < 10; i++)
	{
		if (i < 3) attron(COLOR_PAIR(10));
		else if (i < 6) attron(COLOR_PAIR(11));
		else attron(COLOR_PAIR(12));
		
		if (ratio0 >= i)
			mvprintw(BASE_Y+10-i, BASE_X, "   ");
		else 
			ELSE(0);
		if (ratio1 >= i)
			mvprintw(BASE_Y+10-i, BASE_X+6, "   ");
		else 
			ELSE(6);
		if (ratio2 >= i)
			mvprintw(BASE_Y+10-i, BASE_X+12, "   ");
		else 
			ELSE(12);
		if (ratio3 >= i)
			mvprintw(BASE_Y+10-i, BASE_X+18, "   ");
		else 
			ELSE(18);
	}
	
}


