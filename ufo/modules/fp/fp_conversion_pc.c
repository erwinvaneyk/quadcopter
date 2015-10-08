#include "fp_conversion_pc.h"
#include "fp_conversion.h"
#include "fp.h"

int convertFloatToFP(float value) {
	return value * FRACTION_MAX_VALUE;
}

float convertFPToFloat(int fpValue) {
	return fpValue / FRACTION_MAX_VALUE;
}

