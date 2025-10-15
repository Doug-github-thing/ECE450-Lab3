#include "sobel.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <hls_stream.h>
#include <ap_axi_sdata.h>


// UUT top
void sobel (hls::stream<axis8_t> &in_stream, hls::stream<axis8_t> &out_stream);


// Simple PPM reader/writer
bool read_ppm(const char* filename, ap_uint<8> img[HEIGHT][WIDTH]) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Input does not exist" << std::endl;
        return false;
    }

    std::string header;
    int w, h, maxval;
    file >> header >> w >> h >> maxval;
    file.ignore(1); // skip newline
    if (header != "P5" || w != WIDTH || h != HEIGHT || maxval != 255) {
        std::cout << "Incorrect file header" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(img), WIDTH * HEIGHT);
    return true;
}


bool write_ppm(const char* filename, ap_uint<8> img[HEIGHT][WIDTH]) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file << "P5 " << WIDTH << " " << HEIGHT << " 255\n";
    file.write(reinterpret_cast<char*>(img), WIDTH * HEIGHT);
    return true;
}


int main() {

    std::cout << "!!!! Starting test !!!!" << std::endl;
    
    pix8_t in_bytes[512][512];
    pix8_t out_bytes[512][512];
    
    hls::stream<axis8_t> in_stream;
    hls::stream<axis8_t> out_stream;

    if(!read_ppm(INPUT_PATH, in_bytes))
        return -1;

    // Push grayscale pixels into AXI4-Stream (scanline order)
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            axis8_t packet;
            
            // Set the 8-bit grayscale pixel data
            packet.data = in_bytes[y][x];
            
            // AXI4-Stream sideband signals
            packet.keep = 0x1;  // 1 byte valid
            packet.strb = 0x1;
            packet.user = (x == 0 && y == 0) ? 1 : 0;   // Start of Frame (SOF)
            packet.last = (x == WIDTH - 1) ? 1 : 0;     // End of Line (EOL)
            packet.id   = 0;
            packet.dest = 0;

            in_stream.write(packet);
        }
    }
    
    // 2) Run the DUT
    sobel(in_stream, out_stream);

    // 3) Collect output pixels from AXI4-Stream
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            axis8_t pkt = out_stream.read();
            out_bytes[y][x] = pkt.data;
        }
    }
    
    // Write the result image
    if(!write_ppm(OUTPUT_PATH, out_bytes))
        return -1;

    std::cout << ".... Finshing test ...." << std::endl;

    return 0;
}
