#include <math.h>
#include "fp_conversion.h"

float_x32 convertIntFloatToFP(int decimalPart, int fractalPart, int fractalOffset) {
	int lpart, rpart;
	lpart = convertIntToFP(decimalPart);
	rpart = convertIntToFP(fractalPart) / pow(10, fractalOffset + 1);
	return lpart + rpart;
}

float_x32 convertIntToFP(int value) {
	return value * FRACTION_MAX_VALUE;
}

int convertFPToInt(float_x32 fpValue) {
	return fpValue / FRACTION_MAX_VALUE;
}
