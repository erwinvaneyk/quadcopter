#include "fp_conversion_pc.h"
#include "fp_conversion.h"
#include "fp.h"

float_x32 convertFloatToFP(float value) {
	return value * FRACTION_MAX_VALUE;
}

float convertFPToFloat(float_x32 fpValue) {
	return fpValue / FRACTION_MAX_VALUE;
}

