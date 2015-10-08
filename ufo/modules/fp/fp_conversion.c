#include <math.h>
#include "fp_conversion.h"

int convertIntFloatToFP(int decimalPart, int fractalPart, int fractalOffset) {
	int lpart, rpart;
	lpart = convertIntToFP(decimalPart);
	rpart = convertIntToFP(fractalPart) / pow(10, fractalOffset + 1);
	return lpart + rpart;
}

int convertIntToFP(int value) {
	return value * FRACTION_MAX_VALUE;
}

int convertFPToInt(int fpValue) {
	return fpValue / FRACTION_MAX_VALUE;
}
