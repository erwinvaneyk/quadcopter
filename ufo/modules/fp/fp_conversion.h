// Safe to use on FPGA

#ifndef FP_CONVERSION_H_
#define FP_CONVERSION_H_

#include "fp.h"

/**
 * Usage: To convert a float
 * 1.034 -> convertFloatToFP(1, 34, 1)
 * 
 * fractalPart >= 0
 * fractalOffset >= 0
 */ 
int convertIntFloatToFP(int decimalPart, int fractalPart, int fractalOffset);

int convertIntToFP(int number);

int convertFPToInt(int fpValue);

#endif