// Safe to use on FPGA

#ifndef FP_CONVERSION_H_
#define FP_CONVERSION_H_
/*
 * Interface for converting integers to and from the float_x32 domain.
 */

#include "fp.h"

/**
 * Usage: To convert a float
 * 1.034 -> convertFloatToFP(1, 34, 1)
 * 
 * fractalPart >= 0
 * fractalOffset >= 0
 */ 
float_x32 convertIntFloatToFP(int decimalPart, int fractalPart, int fractalOffset);

float_x32 convertIntToFP(int number);

int convertFPToInt(float_x32 fpValue);

#endif