#include "sobel.h"
#include <cmath>
#include <hls_math.h>

/**
 * Sobel flowchart:
 * 
 * Define sobel kernels
 * 
 * Iterate through every pixel except for the image edges:
 * for each pixel:
 *    Populate Gx[at that pixel] by applying sobel operator on left and right sides:
 *        Gx[pixel] = [1,2,1] applied to the right - [1,2,1] applied to the left
 *    Populate Gy[at that pixel] by applying sobel operator above and below:
 *        Gy[pixel] = [1,2,1] applied to below - [1,2,1] applied to above
 
 *    G = sqrt(Gx^2 + Gy^2)
 * 
 *    Normalize values to 255 scale
 */

void sobel (ap_uint<8> in[WIDTH][HEIGHT], ap_uint<8> out[WIDTH][HEIGHT])
{
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Define Sobel kernels.
    // Typically they're shown as 3x3 matrix with 0s in the middle. That's wasted space.
    
    RowsLoopCalc: for (ap_uint<10> i=1; i<HEIGHT-1; ++i) {
        ColsLoopCalc: for (ap_uint<10> j=1; j<WIDTH-1; ++j) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8

            // Sobel kernel (1,2,1) to the left and right, subtracted
            ap_uint<10> left_side = (in[i-1][j+1])+(2*in[i][j+1])+(in[i+1][j+1]); 
            ap_uint<10> right_side = (in[i-1][j-1])+(2*in[i][j-1])+(in[i+1][j-1]); 
            ap_int<11> Gx = right_side - left_side;
            
            // Sobel kernel (1,2,1) to the below and top, subtracted
            ap_uint<10> bottom_side = (in[i+1][j-1])+(2*in[i+1][j])+(in[i+1][j+1]);
            ap_uint<10> top_side = ((in[i-1][j-1])+(2*in[i-1][j])+(in[i-1][j+1]));
            ap_int<11> Gy = bottom_side - top_side;

            // Use approximation since exact sqrt value is not vital for edge detection 
            // Use G ~= 0.707 * (|Gx| + |Gy|) instead of the slow pythagorean calculation
            ap_uint<11> absGx = hls::abs(Gx);
            ap_uint<11> absGy = hls::abs(Gy);
            // Multiply by 181 and divide by 256 instead of 0.707, makes cleaner int operation
            ap_uint<12> G = (absGx + absGy) * 181 >> 8;

            // Normalize back to 255 range, clamping max values at a chosen value
            if (G >= NORMALIZATION_FACTOR)
                G = 255;
            out[i][j] = (G  * 255 / NORMALIZATION_FACTOR);
        }
    }
}
