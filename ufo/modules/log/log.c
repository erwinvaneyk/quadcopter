#include <stdio.h>
#include "log.h"

struct LOG log[LOG_LENGTH];
int log_counter;

void logs_send() {
	int i = 0;
	printf("********SENDING LOG DATA!**********\n");
	for (i = 0; i < log_counter; i++)
	{
		printf("%d ", log[i].timestamp );
		printf("%d ", log[i].ae[0] );
		printf("%d ", log[i].ae[1] );
		printf("%d ", log[i].ae[2] );
		printf("%d ", log[i].ae[3] );
		printf("%d ", log[i].s[0] );
		printf("%d ", log[i].s[1] );
		printf("%d ", log[i].s[2] );
		printf("%d ", log[i].s[3] );
		printf("%d ", log[i].s[4] );
		printf("%d ", log[i].s[5] );
		printf("%d ", log[i].lift_point );
		printf("\n");
	}
	printf("\n");
	printf("$"); //signal end of transmission
}

void addLog(int timestamp, int *ae, int sax, int say, int saz, int sp, int sq, int sr, int lift_setpoint_rpm) {
	if ((log_counter < LOG_LENGTH)) {
		log[log_counter].timestamp = timestamp; 
		log[log_counter].ae[0] = (uint16_t) ae[0];
		log[log_counter].ae[1] = (uint16_t) ae[1];
		log[log_counter].ae[2] = (uint16_t) ae[2];
		log[log_counter].ae[3] = (uint16_t) ae[3];
		log[log_counter].s[0] = sax;
		log[log_counter].s[1] = say;
		log[log_counter].s[2] = saz;
		log[log_counter].s[3] = sp;
		log[log_counter].s[4] = sq;
		log[log_counter].s[5] = sr;
		log[log_counter].lift_point = lift_setpoint_rpm;
		log_counter++;
	}
}