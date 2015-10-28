#ifndef FP_CONVERSION_PC_H_
#define FP_CONVERSION_PC_H_

/*
 * Conversions from actual floats to float_x32.
 * Note that this interface can only be used by devices that
 * have floating point support.
 */ 

#include "fp.h"

float_x32 convertFloatToFP(float value);

float convertFPToFloat(float_x32 fpValue);

#endif