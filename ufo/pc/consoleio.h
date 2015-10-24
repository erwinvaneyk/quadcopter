#ifndef CONSOLEIO_H_
#define CONSOLEIO_H_

/*
 * Interface for interacting with the terminal
 */

void term_initio();
void term_exitio();
void term_putchar(char c);
int	term_getchar_nb();
int	term_getchar();
void term_puts(char *s);
void term_putchar(char c);

#endif