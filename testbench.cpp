#include <ap_int.h>

const int size = 256;
const int N_TAPS = 16;

const ap_int<4> co_eff[] = {1, 2, 0, -3, 0, 4, -5, 0, 1, -2, 0, -3, 0, 4, -5, 0};

int shift_reg[N_TAPS];

void fir_filter(int in[size], int out[size]) {

    // #pragma HLS PIPELINE
    
	// total filter size start
    Outer:for (int i=0; i<size; ++i) {
        // #pragma HLS UNROLL factor=4
        #pragma HLS PIPELINE

        // shifting accumulation points
        SHIFT:for (int k=N_TAPS-1; k>0; --k) {
            // #pragma HLS UNROLL factor=4
            // #pragma HLS PIPELINE
            shift_reg[k] = shift_reg[k-1];
        }

        shift_reg[0] = in[i];

        long acc = 0;
        ACC:for (int k=N_TAPS-1; k>0; --k) {
            // #pragma HLS UNROLL factor=4
            // #pragma HLS PIPELINE
            acc += shift_reg[k] * co_eff[k];
        }

        out[i] = (int) acc;

    } // total filter size end

}
