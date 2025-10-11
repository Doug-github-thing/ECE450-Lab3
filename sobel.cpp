#include <ap_int.h>

#define WIDTH 512
#define HEIGHT 512

void sobel (ap_int<8> in_bytes[WIDTH][HEIGHT], ap_int<8> out_bytes[WIDTH][HEIGHT])
{
    #pragma HLS INTERFACE ap_none port=in_bytes
    #pragma HLS INTERFACE ap_none port=out_bytes
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Simple starter code for testing the testbench before implementation of filter begins
    RowsLoop: for (int i=0; i<HEIGHT; ++i) {
        ColsLoop: for (int j=0; j<WIDTH; ++j) {
            out_bytes[i][j] = in_bytes[i][j];
        }
    }
    
}
