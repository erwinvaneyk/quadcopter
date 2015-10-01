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

#include "rs232.h"
#include "protocol.h"
#include "consoleio.h" 
#include "joystickio.h"
#include "input.h"
#include "keyboardio.h"

int serial_device = 1;
int fd;
char c;
bool DEBUG;
bool ENABLE_JOYSTICK;

FILE *f;

// Input models
struct js_event js;
struct JOYSTICK joystick;
struct INPUT joystickInput;
struct INPUT keyboardInput;
struct INPUT inputModel;


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
	processKeyboardEvent(c, &keyboardInput);
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

	term_puts("Quadcopter terminal\n-----------------------\nType ./term --help for usage details\n");
	
	// fd is for the joystick
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}

	/* 
	 * Check input 
	 */
	bad_input = parseArgs(argc, argv);

	if (bad_input == -1) 
	{
		printHelp();
		return -1;
	}

	/*
	 * init
	 */
	term_initio();
	link_status = rs232_open(serial_device);
	if(link_status == -1) {
		term_puts("FPGA not detected! Connect it to communicate.\n");
	}

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while (link_status > -1 && (c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);
	
	/* Timer init
	*/
	struct timeval timer1, timer2;

/*debug
c=' ';
f = fopen("log", "w");
if (f == NULL)
{
    printf("Error opening file!\n");
    exit(1); //change this
}
*/
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
						input_to_pkt(&inputModel, &pkt);
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
			input_to_pkt(&inputModel, &pkt);
			inputModel.updated = false;
			//show_input(&inputModel);
			//show_pkt(&pkt);
		}
		
		// Send the packet periodically
		gettimeofday(&timer1, NULL);
		periodic_send (&timer1, &timer2, &pkt, link_status);

		if (link_status > -1 && (c = rs232_getchar_nb()) != -1) 
			term_putchar(c);
			//fprintf(f, "%c", c); //debugs
		//else {printf("error\n");}
	} //end of inf. loop


fclose(f);

		term_exitio();
		if(link_status > -1) {
			rs232_close();
		}
		term_puts("\n<exit>\n");
		return 0;
}