#include <ap_int.h>
#include <ap_axi_sdata.h>

#define INPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_in_greyscale.ppm"
#define OUTPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_out.ppm"

#define WIDTH 512
#define HEIGHT 512

typedef ap_axiu<8,1,1,1> axis8_t;
typedef ap_uint<8> pix8_t;

