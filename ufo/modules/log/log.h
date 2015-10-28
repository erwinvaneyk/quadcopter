#ifndef LOG_H_
#define LOG_H_

/*
 * Logging header file that is used on the fpga.
 */

#include <stdint.h>

struct LOG {
		int timestamp;
		uint16_t ae[4];
		int s[6]; //sensor readings
		int lift_point;
	}; //__attribute__ ((packed))
#endif