#include <stdio.h>
#include <stdlib.h>


#include "filter.h"

int filtered, nofilter, filteredMinusOne;


void butterworth_filter (void) {
    s5_filtered=((b0*s5)) + ((b1*s5_old)) + ((a1*s5_filtered_old) );
    nofilter=filtered;
    filteredMinusOne=s5_filtered;
}


void init_filter(void)
{
	
	nofilter = 0 
	filteredMinusOne =0;
}
