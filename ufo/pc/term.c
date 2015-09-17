/*------------------------------------------------------------
 * Simple terminal in C
 * 
 * Arjan J.C. van Gemund (+ few mods by Mark Dufour)
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

int serial_device = 0;
int fd;
char	c;
int wait;

// Input models
struct js_event js;
struct JOYSTICK joystick;
struct INPUT joystickInput;
struct INPUT keyboardInput;
struct INPUT inputModel;


int getSerialDevice(char **argv) {
	if (strncmp(argv[1],"serial",3) == 0)
		return 0;
	else if (strncmp(argv[1],"usb",3) == 0)
		return 1;
	else if (strncmp(argv[1],"wifi",3) == 0)
		return 2;
	else
		return -1;
}

void    mon_delay_ms(unsigned int ms)
{
        struct timespec req, rem;

        req.tv_sec = ms / 1000;
        req.tv_nsec = 1000000 * (ms % 1000);
        assert(nanosleep(&req,&rem) == 0);
}

void processInput() {
	if(wait <= 0) {
		processJoystickEvent(fd, js, &joystick);
		wait = 10000;
	}
	wait--;
	processKeyboardEvent(c, &keyboardInput);
	updateJoystickInputModel(&joystickInput, &joystick);
	updateInputModel(&inputModel, &keyboardInput, &joystickInput);
}

/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	struct PACKET pkt;
	int 	bad_input = 0;
	int link_status;
	// fd is for the joystick
	if ((fd = open(JS_DEV, O_RDONLY)) < 0) {
		perror("jstest");
		exit(1);
	}

	
	/* Check input 
	 */
	if (argc == 1) 
    		serial_device = 1; 
	
	else if (argc == 2) 
	{
		serial_device = getSerialDevice(argv);
		if(serial_device == -1) {
			bad_input = 1;
		}
	} 
	else 
		bad_input = 1;

	if (bad_input == 1) 
	{
		fprintf(stderr,"Usage: ./term [serial|usb|wifi]\n"); 
		return -1;
	}

	term_puts("\nTerminal program (Embedded Software Lab), ");

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
	
	/* send & receive
	 */

	int level = 0;
	
	for (;;) 
	{
		// Delay for 100 ms
		//mon_delay_ms(100);
		/*if ((c = term_getchar_nb()) != -1) 
			rs232_putchar(c);
			*/
		// Check up on joystick
		//1show_joystick(&joystick);
		// Check keyboard

		// process input (new style :D)
		processInput();
		if(inputModel.updated) {
			#ifdef DEBUG
				show_input(&inputModel);
			#endif
			// Send packet TODO: periodically
			input_to_pkt(&inputModel, &pkt);
			if(link_status > -1) {
				show_pkt(&pkt);
				rs232_put_pkt(&pkt); //if we are sending out things periodically, we might want to do this sometime later
			}
			inputModel.updated = false;
		}
		if (link_status > -1 && (c = rs232_getchar_nb()) != -1) 
			term_putchar(c);
	}

	term_exitio();
	if(link_status > -1) {
		rs232_close();
	}
	term_puts("\n<exit>\n");
  	
	return 0;
}