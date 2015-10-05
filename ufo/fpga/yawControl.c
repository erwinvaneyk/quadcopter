#include <stdio.h>
#include <stdlib.h>


#include "filter.h"


void yaw_control()
{
	
	DISABLE_INTERRUPT(GLOBAL_INTERRUPT);

	filter();
	

	
 
	ae[0] = lift - (yaw-filter)*Pyo;
	ae[1] = ae[1];
	ae[2] = lift + (yaw-filter)*Pyo;
	ae[4] = ae[2];

	ENABLE_INTERRUPT(GLOBAL_INTERRUPT);
}
