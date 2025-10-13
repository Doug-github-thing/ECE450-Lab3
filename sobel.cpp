#include "sobel.h"
#include <hls_math.h>
#include <hls_stream.h>

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

void sobel (hls::stream<ap_uint<8>> &in_stream, hls::stream<ap_uint<8>> &out_stream)
{
    // Stream the data bytes in and out
    #pragma HLS INTERFACE axis port=in_stream
    #pragma HLS INTERFACE axis port=out_stream
    
    ap_uint<8> in[HEIGHT][WIDTH];
    
    // Read input from stream directly to internal array for parallelizable processing later
    InitialRowLoop: for (ap_uint<10> i=0; i<HEIGHT; ++i) {
        InitialColLoop: for (ap_uint<10> j=0; j<WIDTH; ++j) {
            in[i][j] = in_stream.read();
        }
    }

    // Process AND write immediately to prevent lockup during cosim
    ProcessingRowLoop: for (ap_uint<10> i=0; i<HEIGHT; ++i) {
        ProcessingColLoop: for (ap_uint<10> j=0; j<WIDTH; ++j) {
            ap_uint<8> out_pixel = 0;
            
            // if logic is to restrict sobel operator for use not on the outer square.
            // Needed since the loop iterates over the whole image instead of just the inside square.
            if (i>0 && i<HEIGHT-1 && j>0 && j<WIDTH-1) {
                // Your exact Sobel calculation here
                ap_uint<10> right_side = (in[i-1][j+1])+(2*in[i][j+1])+(in[i+1][j+1]); 
                ap_uint<10> left_side =  (in[i-1][j-1])+(2*in[i][j-1])+(in[i+1][j-1]); 
                ap_int<11> Gx = right_side - left_side;
                
                ap_uint<10> bottom_side = (in[i+1][j-1])+(2*in[i+1][j])+(in[i+1][j+1]);
                ap_uint<10> top_side = ((in[i-1][j-1])+(2*in[i-1][j])+(in[i-1][j+1]));
                ap_int<11> Gy = bottom_side - top_side;

                ap_uint<11> absGx = hls::abs(Gx);
                ap_uint<11> absGy = hls::abs(Gy);
                ap_uint<12> G = (absGx + absGy) * 181 >> 8;

                if (G >= NORMALIZATION_FACTOR)
                    G = 255;
                out_pixel = (G * 255 / NORMALIZATION_FACTOR);
            }
            
            out_stream.write(out_pixel);
        }
    }
}