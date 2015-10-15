// Safe to use on FPGA

#ifndef FP_H_
#define FP_H_

/* Hi there! Looking for floating-point support on your fpga?
 * 
 * Suppose f is your floating point value. You will need to convert
 * it to a fixed point notation X with n bit precision.
 * 
 * X = round( f * 2^n )
 *
 * To convert the fixed-point notation X back to the floating point f:
 * 
 * f = X * 2^(-n)
 *
 * On the pc side you can make use of the fp_conversion to do conversions
 * between floats and fixed-point representation.
 * 
 * On the FPGA side you can make use of the fp_arithmetic to do arithmetic
 * on the 
 *
 */

typedef int float_x32;

#define FRACTION_BIT_COUNT 	14 // 14 bit precision gives us 4 digit precision
#define FRACTION_MAX_VALUE  16384 // 2^FRACTION_BIT_COUNT

#endif