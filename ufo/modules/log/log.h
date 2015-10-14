/* log.h */
#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>

#define LOG_LENGTH 10000	//=10 seconds of logging in Manual Mode at 1000Hz
#define NULL 0

struct LOG {
	int timestamp;
	uint16_t ae[4];
	int s[6]; //sensor readings
	int lift_point;
}; //__attribute__ ((packed))

void logs_send();

void addLog(int timestamp, int *ae, int sax, int say, int saz, int sp, int sq, int sr, int lift_setpoint_rpm);

#endif