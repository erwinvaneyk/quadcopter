#include <stdio.h>
#include "fp_arithmetic.h"

// NOTE: multiplication has an error of 1% to avoid overflow.
float_x32 fp_mul(float_x32 c, float_x32 d) {
	int result = (c >> FRACTION_BIT_COUNT / 2) * (d >> FRACTION_BIT_COUNT / 2);
	return result;
}

float_x32 fp_add(float_x32 c, float_x32 d) {
	return c + d;
}

float_x32 fp_sub(float_x32 c, float_x32 d) {
	return c - d;
}