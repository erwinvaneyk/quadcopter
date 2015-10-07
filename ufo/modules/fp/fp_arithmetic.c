#include <stdio.h>
#include "fp_arithmetic.h"
#include "fp.h"

// WARNING MULTIPLICATION DOES NOT WORK YET
int fp_mul(int c, int d) {
	int result = (c >> FRACTION_BIT_COUNT / 2) * (d >> FRACTION_BIT_COUNT / 2);
	return result;
}

int fp_add(int c, int d) {
	return c + d;
}

int fp_min(int c, int d) {
	return c - d;
}