/*------------------------------------------------------------
 * Simple terminal in C
 * 
 * Arjan J.C. van Gemund (+ few mods by Mark Dufour)
 *------------------------------------------------------------
 */

#define DEBUG 
#define	FALSE		0
#define	TRUE		1

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "rs232.h"
#include "protocol.h"
#include "consoleio.h"
#include <signal.h>

int serial_device = 0;
/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{
	int i;
	struct PACKET pkt;
	int 	bad_input = 0;
	char	c;
	
	/* Check input 
	 */
	if (argc == 1) 
    		serial_device = 1; 
	
	else if (argc == 2) 
	{
		if (strncmp(argv[1],"serial",3) == 0)
			serial_device = 0;

		else if (strncmp(argv[1],"usb",3) == 0)
			serial_device = 1;

		else if (strncmp(argv[1],"wifi",3) == 0)
			serial_device = 2;

		else 
			bad_input = 1;	
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
	rs232_open(serial_device);

	term_puts("Type ^C to exit\n");

	/* discard any incoming text
	 */
	while ((c = rs232_getchar_nb()) != -1)
		fputc(c,stderr);
	
	/* send & receive
	 */

	int level = 0;
	for (;;) 
	{
		/*if ((c = term_getchar_nb()) != -1) 
			rs232_putchar(c);
			*/

		if ((c = term_getchar_nb()) != -1) 
		{
			if (c == 'a') // LIFT UP
			{	
				if (level < 15) //highest level
					{	
						level++;
						generate_pkt(&pkt, MANUAL_MODE, LIFT, level_convert(level));
#ifdef DEBUG
show_pkt(&pkt);
#endif					
					}
				rs232_put_pkt(&pkt); //if we are sending out things periodically, we might want to do this sometime later
			}
				
			else if (c == 'z')  // LIFT DOWN
			{
				if (level>-15)
				{
					level--;
					generate_pkt(&pkt, MANUAL_MODE, LIFT, level_convert(level));
#ifdef DEBUG
show_pkt(&pkt);
#endif	
				}
				rs232_put_pkt(&pkt);
			}
			else
				rs232_putchar(c); //still need to discuss this
		}
	
		if ((c = rs232_getchar_nb()) != -1) 
			term_putchar(c);
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");
  	
	return 0;
}