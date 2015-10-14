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
#include "protocol.h"
#include "consoleio.h" 
#include "joystickio.h"
#include "input.h"
#include "keyboardio.h"
#include "tui.h"

int serial_device = 1;
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
	printf("Usage: ./term [args]\n");
	printf("-d, --debug \t Set to debug mode\n");
	printf("-s, --serial \t Set manual serial device (options: serial, usb, wifi)\n");
	printf("-j, --joystick \t Enable the joystick for input\n");
	printf("-h, --help \t this.\n");
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
				printf("No param found for --serial; ignoring it \n");
			}
		} else if(checkArg(arg,"-d", "--debug") > 0) {
			DEBUG = true;
			printf("Debug mode enabled!\n");
		} else if(checkArg(arg, "-j", "--joystick")) {
			ENABLE_JOYSTICK = true;
			printf("Joystick enabled, note that joystick provides shitty values if not connected!\n");
		} else if(checkArg(arg, "-h", "--help")) {
			printHelp();
			return -1;
		}
		c += 1; 
	}
	return 0;
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
		if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
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

	//init special input
	p_input.current_yaw_p = 1; // must match the initial P value on QR
	p_input.yaw_p = 0;
	p_input.updated = false;

	getch();
	refresh();

	//TUI
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
	mvprintw(3,0, "***ENGINE RPM***|    ***SENSOR values***    | ***CONTROL values***     ");
	attroff(A_UNDERLINE);
	mvprintw(4,0, "AE0 AE1 AE2 AE3 | AX   AY   AZ   P   Q   R  | yaw P                    ");
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

	//TUI_engines_init();

	move(5,0);

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
			//Print Current MODE
			switch(inputModel.mode) {
				case SAFE_MODE_INT:
					TUI_PRINT_MODE(SAFE MODE);
					break;
				case PANIC_MODE_INT:
					TUI_PRINT_MODE(PANIC MODE);
					break;
				case MANUAL_MODE_INT:
					TUI_PRINT_MODE(MANUAL MODE);
					//proof of concept
					//TODO: better parse real values though
					/*ae0 = ae1 = ae2 = ae3 = 65*inputModel.lift;
					
					ae1 = ae1 + 15 * inputModel.roll;
					ae3 = ae3 - 15 * inputModel.roll;
					
					ae0 = ae0 + 15 * inputModel.pitch;
					ae2 = ae2 - 15 * inputModel.pitch;

					ae0 = ae0 + 25 * inputModel.yaw;
					ae1 = ae1 - 25 * inputModel.yaw;
					ae2 = ae2 + 25 * inputModel.yaw;
					ae3 = ae3 - 25 * inputModel.yaw;

					TUI_engines(ae0, ae1, ae2, ae3);*/
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

		
			//check special input
			//without this it doesn't stop, although it should
			//this check can be moved to input_to_pkt // or remove altogether #redundant
			if ((p_input.updated == true) && (inputModel.mode == YAW_CONTROL_INT))
			{
				input_to_pkt(&inputModel, &pkt, &p_input);
				rs232_put_pkt(&pkt);
				inputModel.updated = false;
				p_input.updated = false;
				p_input.yaw_p = 0x00; //reduntand, but just in case
			}

			//if we are logging save to a file
			if (inputModel.mode == SEND_TELEMETRY_INT)
			{
				printf("Saving to a file...\n");
				if (link_status > -1)
					{
						c=' ';
						f = fopen("log", "w");
						if (f == NULL)
						{
						    printf("Error opening file!\n");
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
				printf("LOG saved to file.\n");
				
				inputModel.mode = SAFE_MODE_INT;
			}
			/////
			input_to_pkt(&inputModel, &pkt, &p_input);
			inputModel.updated = false;
			show_input(&inputModel);
			show_pkt(&pkt);
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
					//attron(A_BOLD | A_STANDOUT );
					mvaddch(CURRENT_MSG_CURSOR,cursor++,c);
					//attroff(A_BOLD | A_STANDOUT );
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