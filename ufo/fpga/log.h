/* log.h */
#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#define NULL 0

struct LOG {
		uint8_t ae0;
		uint8_t ae1;
		uint8_t ae2;
		uint8_t ae3;
		int s0;
		int s1;
		int s2;
	}; //__attribute__ ((packed))


int allocate_log(struct LOG* log, int log_length); //return -1 if unsuccessful

#endif