#include "sobel.h"
#include <cmath>

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

void sobel (uint8_t in[WIDTH][HEIGHT], uint8_t out[WIDTH][HEIGHT])
{
    #pragma HLS INTERFACE ap_none port=in
    #pragma HLS INTERFACE ap_none port=out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    /**
    // Define Sobel kernels.
    // Typically they're shown as 3x3 matrix with 0s in the middle. That's wasted space.
    
    // Simple starter code for testing the testbench before implementation of filter begins
    RowsLoop: for (int i=1; i<HEIGHT-1; ++i) {
        ColsLoop: for (int j=1; j<WIDTH-1; ++j) {

            // Max possible value happens when right and below pixel values are 255 and left and above pixels are 0.
            // The result is Gx = Gy = 255+2*255+255 = 1020. Only requires 10 bits
            // Final G value is G = sqrt(2*1020^2) = 1443

            // Sobel kernel (1,2,1) to the left and right, subtracted
            int Gx = (in[i-1][j+1])+(2*in[i][j+1])+(in[i+1][j+1])
                  - ((in[i-1][j-1])+(2*in[i][j-1])+(in[i+1][j-1]));
            
            // Sobel kernel (1,2,1) to the below and top, subtracted
            int Gy = (in[i+1][j-1])+(2*in[i+1][j])+(in[i+1][j+1]) 
                  - ((in[i-1][j-1])+(2*in[i-1][j])+(in[i-1][j+1]));
            
            // Hypotenuse between x and y axis
            ap_int<9> G = sqrtf(Gx*Gx + Gy*Gy);

            // Normalize back to 255 range
            out[i][j] = (G  * 255 / 1443);
        }
    }
    */

    ap_int<3> Kx[3][3] = {{-1, 0, 1},
                          {-2, 0, 2},
                          {-1, 0, 1}};
    ap_int<3> Ky[3][3] = {{-1,-2,-1},
                          { 0, 0, 0},
                          { 1, 2, 1}};

    for (int y=1; y<HEIGHT-1; ++y) {
        for (int x=1; x<WIDTH-1; ++x) {
            // For each pixel of the original, apply 3x3 kernel to its neighbors

            int Gx=0; int Gy=0;

            for (int i=-1; i<=1; ++i)
                for (int j=-1; j<=1; ++j) {
                    Gx += in[y+i][x+j] * Kx[1+i][1+j];
                    Gy += in[y+i][x+j] * Ky[1+i][1+j];
                }
            
            // Hypotenuse between x and y axis
            int G = sqrtf(Gx*Gx + Gy*Gy);

            // Normalize back to 255 range, clamping max values at a chosen value
            if (G >= NORMALIZATION_FACTOR)
                G = 255;
            G = (G  * 255 / NORMALIZATION_FACTOR);
            out[y][x] = G;
        }
    }
}
