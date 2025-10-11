#include <ap_int.h>

void compute_c (ap_int<8> in_bytes[512][512], ap_int<8> out_bytes[512][512])
{
    #pragma HLS INTERFACE ap_none port=in_bytes
    #pragma HLS INTERFACE ap_none port=out_bytes
    // #pragma HLS INTERFACE ap_ctrl_none port=return <- Not applicable since this circuit requires control flow

    // Simple starter code for testing the testbench before implementation of filter begins
    out_bytes = in_bytes;
}
