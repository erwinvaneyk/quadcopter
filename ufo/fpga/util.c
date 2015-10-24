/*
 * Utility functions used in the quadcopter
 */
#include "defines.h"

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
 * epileptic_delay_ms -- busy-wait for ms milliseconds while flashing leds
 *------------------------------------------------------------------
 */
void epileptic_delay_ms(int ms) 
{
	int time = X32_ms_clock;
	while(X32_ms_clock - time < ms) {
		if(X32_ms_clock % 20 == 0) {
			toggle_led(0);
		}
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

/*------------------------------------------------------------------
 * within_bounds -- clips x between some lower and upper limit
 *------------------------------------------------------------------
 */
int within_bounds(int x, int lower_limit, int upper_limit) {
	if(x > upper_limit) {
		return upper_limit;
	}
	if(x < lower_limit) {
		return lower_limit;
	}
	return x;
}

/*------------------------------------------------------------------
 * toggle_led -- toggle led # i
 *------------------------------------------------------------------
 */
void toggle_led(int i) 
{
	X32_leds = (X32_leds ^ (1 << i));
}

/*------------------------------------------------------------------
 * reset_leds -- reset all leds
 *------------------------------------------------------------------
 */
void reset_leds() {
	X32_leds = 0;
}
