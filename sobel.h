#include <ap_int.h>

#define INPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_in_greyscale.ppm"
#define OUTPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_out.ppm"

#define WIDTH 512
#define HEIGHT 512

// Chosen experimentially to provide good brightness contrast on engine.png.
// Theoretical max (about 1443) produces a dark output that's hard to read.
// 
// Max possible value happens when right and below pixel values are 255 and left and above pixels are 0.
// The result is Gx = Gy = 255+2*255+255 = 1020. Only requires 10 bits
// Final G value is G = sqrt(2*1020^2) = 1443
// Since that max value is rare, a custom normalization factor was chosen for image contrast.
#define NORMALIZATION_FACTOR 600
