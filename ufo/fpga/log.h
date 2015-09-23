/* log.h */
#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#define NULL 0

struct LOG {
		int timestamp;
		uint8_t ae[4];
		/*uint8_t ae1;
		uint8_t ae2;
		uint8_t ae3;
		*/
		int s0;
		int s1;
		int s2;
	}; //__attribute__ ((packed))


//void log_now(struct LOG logp[], int ae0, int ae1, int ae2, int ae3); 

#endif