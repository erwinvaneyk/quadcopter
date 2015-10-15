// Safe to use on FPGA

#ifndef FP_ARITHMETIC_H_
#define FP_ARITHMETIC_H_

#include "fp.h"

float_x32 fp_mul(float_x32 c, float_x32 d);

float_x32 fp_add(float_x32 c, float_x32 d);

float_x32 fp_sub(float_x32 c, float_x32 d);
#endif