#include <stdio.h>

void fir_filter( int in[256], int out[256]);

int main() {
	int input[256], output[256];

	// Define initial test wave input
	for (int i=0; i<256; ++i)
		input[i] = i;

	// Run it through the filter
	fir_filter(input, output);

	// Print the results
	for (int i=0; i<256; ++i)
		printf("result = %d\n", output[i]);

	// have to check the final output value AFTER completion of the loop.
	// If the filter does not work as intended, the TB will terminate prematurely
	if (output[255] != -1452)
		return 1;
	return 0;
}
