#include "defines.h"

/*------------------------------------------------------------------
 * toggle_led -- toggle led # i
 *------------------------------------------------------------------
 */
void toggle_led(int i) 
{
	X32_leds = (X32_leds ^ (1 << i));
}


/*------------------------------------------------------------------
 * led -- enable/disable led #i
 *------------------------------------------------------------------
 */
void led(int i, int value) {
	if(value <= 0) {
		X32_leds = (X32_leds | (1 << i));
	} else {
		X32_leds = (X32_leds & (0 << i));
	}
}

/*------------------------------------------------------------------
 * delay_ms -- busy-wait for ms milliseconds
 *------------------------------------------------------------------
 */
void delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms);
}

/*------------------------------------------------------------------
 * epileptic_delay_ms -- Will annoying flash leds during busy-wait
 *------------------------------------------------------------------
 */
void epileptic_delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms) {
		if(X32_ms_clock % 10 == 0)
		toggle_led(0);
	}
}

/*------------------------------------------------------------------
 * delay_us -- busy-wait for us microseconds
 *------------------------------------------------------------------
 */
void delay_us(int us) 
{
	int time = X32_us_clock;
	while(X32_us_clock - time < us);
}
