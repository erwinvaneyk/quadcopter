/*------------------------------------------------------------
 * Simple terminal in C
 * 
 * Arjan J.C. van Gemund (+ few mods by Mark Dufour)
 *------------------------------------------------------------
 */
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

int serial_device = 0;
/*----------------------------------------------------------------
 * main -- execute terminal
 *----------------------------------------------------------------
 */
int main(int argc, char **argv)
{

//USAGE
//void generate_pkt(struct pkt* packet, uint8_t mode, uint8_t command, uint16_t data);
	int i;
	struct PACKET pkt;
	//generate_pkt(&pkt, MANUAL_MODE, LIFT, LEVEL1);
	//show_pkt(&pkt);
//	rs232_put_pkt(&pkt);

/*
	printf ("SIZE: %d\n", sizeof(pkt));
	char *tx = (char*)&pkt;
	for (i=0; i<sizeof(pkt); i++)
		{
			printf("%x ", tx[i]);
		}
*/
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
				//also need to make a function that will automatically
				//take care of leveling up or down,
				//depending on the current situatuon
				if (level == 0) //hover
					{
						generate_pkt(&pkt, MANUAL_MODE, LIFT, LEVEL1);
						level++;
					}
				else if (level == 1)
					{
						generate_pkt(&pkt, MANUAL_MODE, LIFT, LEVEL2);
						level++;
					}
				rs232_put_pkt(&pkt);
			}
				
			else if (c == 'z')  // LIFT DOWN
			{
				if (level == 2)
				{
					generate_pkt(&pkt, MANUAL_MODE, LIFT, LEVEL1);
					level--;
				}
				else if (level == 1)
				{
					generate_pkt(&pkt, MANUAL_MODE, LIFT, HOVER);
					level--;
				}
				rs232_put_pkt(&pkt);
			}
			else
				rs232_putchar(c);
		}
	

		
		if ((c = rs232_getchar_nb()) != -1) 
			term_putchar(c);
	}

	term_exitio();
	rs232_close();
	term_puts("\n<exit>\n");
  	
	return 0;
}


