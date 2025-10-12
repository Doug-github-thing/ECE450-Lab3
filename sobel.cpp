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
    
    ap_int<11> Gx[WIDTH][HEIGHT];
    ap_int<11> Gy[WIDTH][HEIGHT];

    RowsLoopCalc: for (ap_uint<10> i=1; i<HEIGHT-1; ++i) {
        ColsLoopCalc: for (ap_uint<10> j=1; j<WIDTH-1; ++j) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8

            // Sobel kernel (1,2,1) to the left and right, subtracted
            ap_uint<10> left_side = (in[i-1][j+1])+(2*in[i][j+1])+(in[i+1][j+1]); 
            ap_uint<10> right_side = (in[i-1][j-1])+(2*in[i][j-1])+(in[i+1][j-1]); 
            Gx[i][j] = right_side - left_side;
            
            // Sobel kernel (1,2,1) to the below and top, subtracted
            ap_uint<10> bottom_side = (in[i+1][j-1])+(2*in[i+1][j])+(in[i+1][j+1]);
            ap_uint<10> top_side = ((in[i-1][j-1])+(2*in[i-1][j])+(in[i-1][j+1]));
            Gy[i][j] = bottom_side - top_side;
        }
    }
            
    RowsLoopNormalization: for (ap_uint<10> i=1; i<HEIGHT-1; ++i) {
        ColsLoopNormalization: for (ap_uint<10> j=1; j<WIDTH-1; ++j) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8
            // Hypotenuse between x and y axis
            // Recast G to double width for squaring without distortion
            ap_int<11> this_Gx = Gx[i][j];
            ap_int<11> this_Gy = Gy[i][j];

            ap_uint<22> GG = (ap_int<22>)this_Gx * this_Gx + (ap_int<22>)this_Gy * this_Gx;
            ap_uint<11> G = hls::sqrt(GG); // 2^11=2048 > 1443 max output

            // Normalize back to 255 range, clamping max values at a chosen value
            if (G >= NORMALIZATION_FACTOR)
                G = 255;
            out[i][j] = (G  * 255 / NORMALIZATION_FACTOR);
        }
    }

    // ap_int<3> Kx[3][3] = {{-1, 0, 1},
    //                       {-2, 0, 2},
    //                       {-1, 0, 1}};
    // ap_int<3> Ky[3][3] = {{-1,-2,-1},
    //                       { 0, 0, 0},
    //                       { 1, 2, 1}};

    // // unsigned 10 bit int to traverse 512x512 array
    // RowsLoop: for (ap_uint<10> y=1; y<HEIGHT-1; ++y) {
    //     ColsLoop: for (ap_uint<10> x=1; x<WIDTH-1; ++x) {
    //         // For each pixel of the original, apply 3x3 kernel to its neighbors
    //         #pragma HLS PIPELINE II=1

    //         // Max possible value is Gx = Gy = 255+2*255+255 = 1020. Only requires 11 (signed) bits
    //         ap_int<11> Gx=0; ap_int<11> Gy=0;

    //         KernelY: for (int i=-1; i<=1; ++i)
    //             KernelX: for (int j=-1; j<=1; ++j) {
    //                 #pragma HLS UNROLL factor=3
    //                 Gx += in[y+i][x+j] * Kx[1+i][1+j];
    //                 Gy += in[y+i][x+j] * Ky[1+i][1+j];
    //             }

    //         // Recast G to double width for squaring without distortion
    //         ap_uint<22> GG = (ap_int<22>)Gx * Gx + (ap_int<22>)Gy * Gy;
    //         ap_uint<11> G = hls::sqrt(GG); // 2^11=2048 > 1443 max output

    //         // Normalize back to 255 range, clamping max values at a chosen value
    //         if (G >= NORMALIZATION_FACTOR)
    //             G = 255;
    //         G = (G  * 255 / NORMALIZATION_FACTOR);
    //         out[y][x] = G;
    //     }
    // }
    
}
