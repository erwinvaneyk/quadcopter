/*------------------------------------------------------------
 * QR terminal
 * 
 *------------------------------------------------------------
 */

//#define DEBUG 
#define	FALSE		0
#define	TRUE		1

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <ncurses.h>


#include "rs232.h"
#include "../modules/pkt/pkt_generation.h"
#include "consoleio.h" 
#include "joystickio.h"
#include "input.h"
#include "keyboardio.h"
#include "tui.h"

int serial_device = 1;
char* js_device = "/dev/input/js2";
int fd;
char c;
bool DEBUG;
bool ENABLE_JOYSTICK;
int msg_cursor =0;

FILE *f;

// Input models
struct js_event js;
struct JOYSTICK joystick;
struct INPUT joystickInput;
struct INPUT keyboardInput;
struct INPUT inputModel;

struct SPECIAL_INPUT p_input;

int getSerialDevice(char *argv) {
	if (strncmp(argv,"serial",3) == 0)
		return 0;
	else if (strncmp(argv,"usb",3) == 0)
		return 1;
	else if (strncmp(argv,"wifi",3) == 0)
		return 2;
	else
		return -1;
}

void printHelp() {
	printw("Usage: ./term [args]\n");
	printw("-d, --debug \t Set to debug mode\n");
	printw("-s, --serial \t Set manual serial device (options: serial, usb, wifi)\n");
	printw("-j, --joystick \t Enable the joystick for input\n");
	printw("-h, --help \t this.\n");
}

int checkArg(char *arg, char *shortname, char *longname) {
	if(strcmp(arg, shortname) == 0 || strcmp(arg, longname) == 0) {
		return 1;  
	}
	return 0;
}

int parseArgs(int argc, char **argv) {
	int c = 1;
	char *arg;
	while(c < argc) {
		arg = argv[c];
		if(checkArg(arg,"-s", "--serial") > 0) {
			c += 1;
			if(c < argc) {
				serial_device = getSerialDevice(argv[c]);
			} else {
				printw("No param found for --serial; ignoring it \n");
			}
		} else if(checkArg(arg,"-d", "--debug") > 0) {
			DEBUG = true;
			printw("Debug mode enabled!\n");
		} else if(checkArg(arg, "-j", "--joystick")) {
			c += 1;
			if(c < argc) {
				js_device = argv[c];
			} else {
				printw("No device-param found for --joystick; using device /dev/input/js0 \n");
			}
			ENABLE_JOYSTICK = true;
			printw("Joystick enabled: note that incorrect data is read if not connected at %s!\n", js_device);
		} else if(checkArg(arg, "-h", "--help")) {
			//printHelp();
			return -1;
		}
		c += 1; 
	}
	getch(); //hold on
	return 0;
}

void periodic_send(struct timeval* timer_main, struct timeval* timer_r, struct PACKET* pkt, int link_status)
{
	if ((labs(((timer_main->tv_usec + timer_main->tv_sec*1000000) - (timer_r->tv_usec + timer_r->tv_sec*1000000))) > COMM_T))
	{	
		if(link_status > -1) {
			rs232_put_pkt(pkt);
		}
		//reset timer
		gettimeofday(timer_r, NULL);	
	}
}

void processInput() {
	if(ENABLE_JOYSTICK) {
		processJoystickEvent(fd, js, &joystick);
		updateJoystickInputModel(&joystickInput, &joystick);
	}
	processKeyboardEvent(c, &keyboardInput, &p_input);
	updateInputModel(&inputModel, &keyboardInput, &joystickInput);
}


/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	struct PACKET pkt;
	int bad_input = 0;
	int link_status;
	int cursor;
	int PRINT_MODE = STATUS;
	initscr();
	//int ae0, ae1, ae2, ae3; //needed for engine TUI
	
	//init p values input
	p_input.current_yaw_p = 1; // must match the initial P value on QR
	p_input.yaw_p = 1;
	p_input.full_p1 = 1;
	p_input.full_p2 = 1;
	p_input.sensitivity = 30;
	p_input.updated = true; //this fill force the values to be sent over once


	printw("Quadcopter terminal\n-----------------------\nType ./term --help for usage details\n");
	getch();
	refresh();
	/* 
	 * Check input 
	 */
	bad_input = parseArgs(argc, argv);

	if (bad_input == -1) 
	{
		printHelp();
		getch();
		refresh();
		return -1;
	}
	
	// fd is for the joystick
	if (ENABLE_JOYSTICK)
	{ 
		if ((fd = open(js_device, O_RDONLY)) < 0) {
			perror("jstest");
			exit(1);
		}
	}

	/*
	 * init
	 */
	term_initio();
	link_status = rs232_open(serial_device);
	if(link_status == -1) {
		printw("FPGA not detected! Connect it to communicate.\n");
	}

	printw("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while (link_status > -1 && (c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);
	
	/* Timer init
	*/
	struct timeval timer1, timer2;

	getch();
	refresh();

	TUI_frame_init(); //Initialize the TUI

	/* send & receive
	 */
	for (;;) 
	{
		processInput();
		if(inputModel.updated) {
			if(DEBUG) {
				show_input(&inputModel);
				if(link_status > -1) {
					show_pkt(&pkt);
				}
			}

			//TUI: Print Current MODE
			switch(inputModel.mode)
			{
				case SAFE_MODE_INT:
					TUI_PRINT_MODE(SAFE MODE);
					break;
				case PANIC_MODE_INT:
					TUI_PRINT_MODE(PANIC MODE);
					break;
				case MANUAL_MODE_INT:
					TUI_PRINT_MODE(MANUAL MODE);
					break;
				case YAW_CONTROL_INT:
					TUI_PRINT_MODE(YAW CONTROL MODE);
					break;
				case CALIBRATE_MODE_INT:
					TUI_PRINT_MODE(CALIBRATING);
					break;
				case FULL_CONTROL_INT:
					TUI_PRINT_MODE(FULL CONTROL MODE);
					break;			
			}

			input_to_pkt(&inputModel, &pkt, &p_input);
			inputModel.updated = false;
			//show_input(&inputModel);
			//show_pkt(&pkt);

			//if we are logging save to a file
			if (inputModel.mode == SEND_TELEMETRY_INT)
			{
				TUI_PRINT_MESSAGE(Saving to a file...);
				if (link_status > -1)
				{
					c=' ';
					f = fopen("logs/log", "w");
					if (f == NULL)
					{
						TUI_PRINT_MESSAGE(Error opening file!);
						exit(1); //change this
					}
					//artificailly sent a packet
					input_to_pkt(&inputModel, &pkt, &p_input);
					inputModel.updated = false;
					rs232_put_pkt(&pkt);
					while(c!='$')
					{
						if ((c = rs232_getchar_nb()) != -1)
						{
							fprintf(f, "%c", c);
						}
					}
					fclose(f);
				}
				TUI_PRINT_MESSAGE(LOG saved to file);
				inputModel.mode = SAFE_MODE_INT;
			}
		}
		// Send the packet periodically
		gettimeofday(&timer1, NULL);
		periodic_send (&timer1, &timer2, &pkt, link_status);

		//printing to screen
		if (link_status > -1 && (c = rs232_getchar_nb()) != -1)
		{
			if (c=='$') //msg from QR
			{
				PRINT_MODE = MESSAGE;
				c = rs232_getchar_nb();
			}

			switch(PRINT_MODE) {
				case STATUS:
					attron(COLOR_PAIR(3));
					attron(A_BOLD | A_STANDOUT );
					mvaddch(5,cursor++,c);
					attroff(A_BOLD | A_STANDOUT );
					attroff(COLOR_PAIR(3));
					if (c=='\n') 
					{
						cursor = 0;
						refresh();
					} 
					break;
				case MESSAGE:
					attron(COLOR_PAIR(4));
					mvaddch(CURRENT_MSG_CURSOR,cursor++,c);
					attroff(COLOR_PAIR(4));
					if (c=='\n') 
					{
						TUI_MOVE_CURSOR;
						cursor = 0;
						refresh();
						PRINT_MODE = STATUS;
					} 
				break;
			}			
		}
		//term_putchar(c);
	} //end of inf. loop


	endwin();
	term_exitio();
	if(link_status > -1) {
		rs232_close();
	}
	term_puts("\n<exit>\n");
	return 0;
}