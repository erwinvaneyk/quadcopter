/*------------------------------------------------------------
 * RS232 Application Programming Interface 
 * 
 * <rs232.c>
 *------------------------------------------------------------
 */
#include <termios.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "rs232.h"
#include "../modules/pkt/pkt_generation.h"
 
int rs232_open(int serial_device)
{
  	char *name;
  	int result;  
  	struct termios	tty;

	if (serial_device == 0) 
	{   
		fd_RS232 = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY);
		fprintf(stderr,"using %s\n", SERIAL_DEVICE); 

	} 
	else if ( (serial_device == 1) || (serial_device == 2) ) 
	{
        fd_RS232 = open(USB_DEVICE, O_RDWR | O_NOCTTY);
		fprintf(stderr,"using %s\n", USB_DEVICE); 
	} 

	if(fd_RS232< 0) {
		return -1;
	}

  	result = isatty(fd_RS232);
  	assert(result == 1);

  	name = ttyname(fd_RS232);
  	assert(name != 0);

  	result = tcgetattr(fd_RS232, &tty);	
	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */		

	/* Set output and input baud rates. 
	 */
	if (serial_device == 0 || serial_device == 1) // wired 
	{  
		cfsetospeed(&tty, B115200); 
		cfsetispeed(&tty, B115200); 
	} 
    	else if (serial_device == 2) // wireless 
	{  
		cfsetospeed(&tty, B9600); 
		cfsetispeed(&tty, B9600); 
	}

	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 0;

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
	link_status = result;
	return result;
}


int rs232_close(void)
{
  	if(link_status == RS232_ERROR) return RS232_ERROR;
  	int result = close(fd_RS232);
  	assert (result==0);
  	return result;
}


int	rs232_getchar_nb()
{
	if(link_status == RS232_ERROR) return RS232_ERROR;
	int 		result;
	unsigned char 	c;

	result = read(fd_RS232, &c, 1);

	if (result == 0) 
		return -1;
	
	else 
	{
		assert(result == 1);   
		return (int) c;
	}
}


int rs232_getchar()
{
	if(link_status == RS232_ERROR) return RS232_ERROR;
	int c;
	while ((c = rs232_getchar_nb()) == -1);
	return c;
}


int rs232_putchar(char c)
{ 
	if(link_status == RS232_ERROR) return RS232_ERROR;
	int result;
	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);   

	assert(result == 1);
	return result;
}

int rs232_put_pkt(struct PACKET* packet)
{ 
	if(link_status == RS232_ERROR) return RS232_ERROR;
	int result;
	char *data = (char*)packet;
	do {
		result = (int) write(fd_RS232, data, PACKET_SIZE);
	} while (result == 0);   

	assert(result == PACKET_SIZE);
	return result;
}