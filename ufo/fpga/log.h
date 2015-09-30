/* log.h */
#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#define NULL 0

struct LOG {
		int timestamp;
		uint16_t ae[4];
		int s[6]; //sensor readings
	}; //__attribute__ ((packed))


//void log_now(struct LOG logp[], int ae0, int ae1, int ae2, int ae3); 

#endif