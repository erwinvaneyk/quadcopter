
/*------------------------------------------------------------
 * Serial I/O 
 * 8 bits, 1 stopbit, no parity, 
 * 115,200 baud wired and 9,600 baud wireless.
 *------------------------------------------------------------
 */
 
#include "../modules/pkt/pkt_generation.h"

#define SERIAL_DEVICE	"/dev/ttyS0"
#define USB_DEVICE		"/dev/ttyUSB0"
#define WIFI_DEVICE 	"/dev/ttyUSB0" /* may need to change this */

int fd_RS232;

int rs232_open(int serial_device);
int rs232_close(void);
int	rs232_getchar_nb();
int rs232_getchar();
int rs232_putchar(char c);
int rs232_put_pkt(struct PACKET* packet);