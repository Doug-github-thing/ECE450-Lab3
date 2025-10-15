#include "sobel.h"
#include <hls_math.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

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

/**
 * Parallel Sobel using widened AXI stream:
 * - Input: 16-bit AXI stream -- 16-bits allow reading of 2 pixels per cycle
 * - Output: 16-bit AXI stream -- 16-bits allow writing of 2 pixels per cycle
 * - Uses 3x5 window to compute 2 Sobel operations per cycle
 * - Throughput: 2 pixels/cycle, ~2x speedup over 8-bit version
 */

void sobel(hls::stream<axis16_t> &in_stream, hls::stream<axis16_t> &out_stream) {
    #pragma HLS INTERFACE axis port=in_stream
    #pragma HLS INTERFACE axis port=out_stream

    static ap_uint<8> line0[WIDTH];
    static ap_uint<8> line1[WIDTH];

    // 3 × 5 sliding window to process more pixels per cycle
    ap_uint<8> w0[5]={0,0,0,0,0};
    ap_uint<8> w1[5]={0,0,0,0,0};
    ap_uint<8> w2[5]={0,0,0,0,0};

    // Clear line buffers for this frame
    init_cols:
    for(int x=0; x<WIDTH; ++x){

        line0[x]=0; line1[x]=0;
    }

    // Main scan
    row_loop:
    for(int y=0; y<HEIGHT; ++y) {
        // Reset window at start of each row
        for(int i=0; i<5; ++i) {
            w0[i]=0; w1[i]=0; w2[i]=0;
        }

        col_loop:
        for(int x=0; x<WIDTH; x+=2) {  // Process 2 pixels per iteration
            #pragma HLS PIPELINE II=1

            // Read 2 pixels packed in 16-bit stream
            axis16_t din = in_stream.read();
            pix8_t pix0 = din.data.range(7, 0);   // Lower byte
            pix8_t pix1 = din.data.range(15, 8);  // Upper byte
            
            bool valid1 = (x+1 < WIDTH);

            // Fetch from line buffers
            pix8_t top2_0 = line1[x];
            pix8_t top1_0 = line0[x];
            pix8_t top2_1 = valid1 ? line1[x+1] : (pix8_t)0;
            pix8_t top1_1 = valid1 ? line0[x+1] : (pix8_t)0;

            // Shift window left by 2
            w0[0]=w0[2]; w0[1]=w0[3]; w0[2]=w0[4];
            w1[0]=w1[2]; w1[1]=w1[3]; w1[2]=w1[4];
            w2[0]=w2[2]; w2[1]=w2[3]; w2[2]=w2[4];

            // Insert 2 new columns on the right side
            w0[3] = top2_0; w0[4] = top2_1;
            w1[3] = top1_0; w1[4] = top1_1;
            w2[3] = pix0;   w2[4] = pix1;

            // Update line buffers
            line1[x] = line0[x];
            line0[x] = pix0;
            if(valid1) {
                line1[x+1] = line0[x+1];
                line0[x+1] = pix1;
            }

            // Compute 2 Sobel operations in parallel
            
            // First pixel: using window columns [0,1,2]
            int gx0 = 0, gy0 = 0;
            if(y >= 2 && x >= 2) {
                gx0 = -(int)w0[0] + (int)w0[2]
                      -2*(int)w1[0] + 2*(int)w1[2]
                      -(int)w2[0] + (int)w2[2];
                gy0 = -(int)w0[0] -2*(int)w0[1] - (int)w0[2]
                      +(int)w2[0] +2*(int)w2[1] + (int)w2[2];
            }
            int mag0 = (gx0<0?-gx0:gx0) + (gy0<0?-gy0:gy0);
            if(mag0 > 255) mag0 = 255;
            pix8_t edge0 = (y>=2 && x>=2) ? (pix8_t)mag0 : (pix8_t)0;

            // Second pixel: using window columns [1,2,3]
            int gx1 = 0, gy1 = 0;
            if(y >= 2 && x >= 1 && valid1) {
                gx1 = -(int)w0[1] + (int)w0[3]
                      -2*(int)w1[1] + 2*(int)w1[3]
                      -(int)w2[1] + (int)w2[3];
                gy1 = -(int)w0[1] -2*(int)w0[2] - (int)w0[3]
                      +(int)w2[1] +2*(int)w2[2] + (int)w2[3];
            }
            int mag1 = (gx1<0?-gx1:gx1) + (gy1<0?-gy1:gy1);
            if(mag1 > 255) mag1 = 255;
            pix8_t edge1 = (y>=2 && x>=1 && valid1) ? (pix8_t)mag1 : (pix8_t)0;

            // Pack 2 output pixels into 16-bit stream
            axis16_t dout;
            dout.data.range(7, 0) = edge0;   // Lower byte
            dout.data.range(15, 8) = edge1;  // Upper byte
            
            // Frame markers
            dout.user = (y==0 && x==0) ? 1 : 0;         // SOF
            dout.last = (x>=WIDTH-2) ? 1 : 0;           // EOL (last transaction)
            dout.keep = valid1 ? 0x3 : 0x1;             // Both bytes or just lower
            dout.strb = valid1 ? 0x3 : 0x1;
            dout.id = 0; 
            dout.dest = 0;

            out_stream.write(dout);
        }
    }
}