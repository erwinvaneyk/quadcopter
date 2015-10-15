#include "fp_conversion_pc.h"
#include "fp_conversion.h"
#include "fp_arithmetic.h"
#include "fp.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define EPSILON 0.0001

void printResult(bool succes, char* message) {
	if(succes) {
		printf("OK\t\t");
	} else {
		printf("FAILED\t\t");
	}

	if(message[0]) {
		printf("%s", message);
	}
	printf("\n");
}

bool floatEquals(float a, float b, float epsilon) {
	float result = a - b;
	if(result < 0) {
		result *= -1;
	}
	return result < epsilon;
}

void testConversion() {
	float_x32 result;
	result = convertIntFloatToFP(0, 5, 0);
	if(result != 8192) {
		printResult(false, "convertIntFloatToFP failed");
		return;
	}

	result = convertFPToInt(convertIntToFP(42));
	if(result != 42) {
		printResult(false, "convertIntToFP or convertFPToInt failed");
		return;
	} 

	printResult(true,"fp_conversion");
}

void testConversionPC() {
	float_x32 result;
	float expected = 78.435;
	result = convertFPToFloat(convertFloatToFP(expected));
	printResult(result != expected, "fp_conversion_pc");
}

void testArithmetic() {
	// Setup
	float_x32 a, b, result;
	float expected;
	float inputA = 44.67;
	float inputB = 27.942;
	expected = inputA + inputB;
	a = convertFloatToFP(inputA);
	b = convertFloatToFP(inputB);

	result = fp_add(a,b);
	if(!floatEquals(convertFPToFloat(result), (inputA + inputB), EPSILON)) {
		printf("%f == %f\n", convertFPToFloat(result), inputA * inputB);
		printResult(false, "fp_add failed");
		return;
	}

	result = fp_sub(a, b);
	if(!floatEquals(convertFPToFloat(result), (inputA - inputB), EPSILON)) {
		printf("%f == %f\n", convertFPToFloat(result), inputA * inputB);
		printResult(false, "fp_sub failed");
		return;
	}

	result = fp_mul(a, b);
	if(!floatEquals(convertFPToFloat(result), (inputA * inputB), (inputA * inputB) / 1000)) {
		printf("%f == %f\n", convertFPToFloat(result), inputA * inputB);
		printResult(false, "fp_mul failed");
		return;
	}

	printResult(true,"fp_conversion");
}

int main(int argc, char **argv) {
	 // For the qr constants
	printf("0.0305 -> %x\n", convertFloatToFP(0.0305));
	printf("1 -> %x\n", convertFloatToFP(1));
	printf("0 -> %x\n", convertFloatToFP(0));
	printf("0.00410-> %x\n", convertFloatToFP(0.00410));
	printf("0 -> %x\n", convertFloatToFP(0));

	// Tests
	printf("Running tests...\n");
	if(FRACTION_BIT_COUNT != 14) {
		printResult(false, "Tests made to work with 14 bit precision!");
		return 0;
	}

	testConversion();
	testConversionPC();
	testArithmetic();

	printf("Finished.\n");
}