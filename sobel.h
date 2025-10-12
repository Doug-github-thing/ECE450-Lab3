#include <ap_int.h>

#define INPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_in_greyscale.ppm"
#define OUTPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_out.ppm"

#define WIDTH 512
#define HEIGHT 512

// Chosen experimentially to provide good brightness contrast on engine.png.
// Theoretical max (≈1443) produces a dark output that's hard to read.
#define NORMALIZATION_FACTOR 600
