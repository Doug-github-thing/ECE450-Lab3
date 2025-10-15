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

void sobel (hls::stream<axis8_t> &in_stream, hls::stream<axis8_t> &out_stream) {
    #pragma HLS INTERFACE axis      port=in_stream
    #pragma HLS INTERFACE axis      port=out_stream

    const int MAX_W = WIDTH;
    static ap_uint<8> line0[WIDTH];
    static ap_uint<8> line1[WIDTH];

    // 3x3 sliding window (three rows × three columns)
    ap_uint<8> w0[3]={0,0,0}, w1[3]={0,0,0}, w2[3]={0,0,0};


    // Clear line buffers for this frame
    init_cols:
    for(int x=0; x<WIDTH; ++x){

        line0[x]=0; line1[x]=0;
    }

    // Main scan
    row_loop:
    for(int y=0; y<HEIGHT; ++y){
        // restart window at each new line
        w0[0]=w0[1]=w0[2]=0;
        w1[0]=w1[1]=w1[2]=0;
        w2[0]=w2[1]=w2[2]=0;

        col_loop:
        for(int x=0; x<WIDTH; ++x){

            axis8_t din = in_stream.read();
            pix8_t this_pixel = din.data.range( 7, 0);

            // Fetch previous two rows at this column BEFORE updating
            pix8_t top2 = line1[x];   // y-2
            pix8_t top1 = line0[x];   // y-1

            // Shift window left
            w0[0]=w0[1]; w0[1]=w0[2];
            w1[0]=w1[1]; w1[1]=w1[2];
            w2[0]=w2[1]; w2[1]=w2[2];

            // Insert new rightmost column from the three rows
            w0[2] = top2;
            w1[2] = top1;
            w2[2] = this_pixel;

            // Update line buffers (roll rows down)
            line1[x] = line0[x];
            line0[x] = this_pixel;

            int gx = 0, gy = 0;
            // Only valid after we have at least 3 cols and 3 rows
            if(y >= 2 && x >= 2){
                // Sobel Gx
                gx =  - (int)w0[0] + (int)w0[2]
                      -2*(int)w1[0] + 2*(int)w1[2]
                      - (int)w2[0] + (int)w2[2];
                // Sobel Gy
                gy =  - (int)w0[0] -2*(int)w0[1] - (int)w0[2]
                       + (int)w2[0] +2*(int)w2[1] + (int)w2[2];
            }
            int mag = (gx<0?-gx:gx) + (gy<0?-gy:gy); // Manhattan approximation for sqrt
            if(mag > 255) mag = 255;        // Clip to max intensity
            pix8_t edge = (y>=2 && x>=2) ? (pix8_t)mag : (pix8_t)0;

            axis8_t dout;
            dout.data.range( 7, 0) = edge;  // B

            // Generate frame markers
            dout.user = (y==0 && x==0) ? 1 : 0;         // SOF
            dout.last = (x==WIDTH-1) ? 1 : 0;           // EOL
            dout.keep = din.keep;  dout.strb = din.strb;
            dout.id   = din.id;    dout.dest = din.dest;

            out_stream.write(dout);
        }
    }
}
