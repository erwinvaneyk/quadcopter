#include "fp_conversion.h"

int pow(value, exp) {
	int result = 0;
	if(exp <= 0) {
		// this pow function does not support negative values, so just return 1 in that case
		return 1;
	}
	for(;exp > 0; exp--) {
		result += value;
	}
	return result;
}

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
