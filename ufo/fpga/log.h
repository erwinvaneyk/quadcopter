/* log.h */
#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#define NULL 0

struct LOG {
		int timestamp;
		uint16_t ae[4];
		int s[6]; //sensor readings
		int lift_point;
	}; //__attribute__ ((packed))
#endif