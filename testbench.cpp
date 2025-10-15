#include "sobel.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <hls_stream.h>
#include <ap_axi_sdata.h>


// UUT top
void sobel(hls::stream<axis16_t> &in_stream, hls::stream<axis16_t> &out_stream);


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
    
    pix8_t in_bytes[HEIGHT][WIDTH];
    pix8_t out_bytes[HEIGHT][WIDTH];
    
    hls::stream<axis16_t> in_stream;
    hls::stream<axis16_t> out_stream;

    if(!read_ppm(INPUT_PATH, in_bytes))
        return -1;

    // Push grayscale pixels into AXI4-Stream (2 pixels per transaction)
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; x += 2) {  // CHANGED: increment by 2
            // Set the 16-bit data with 2 adjacent pixels
            axis16_t packet;
            packet.data.range(7, 0) = in_bytes[y][x];
            packet.data.range(15, 8) = (x+1 < WIDTH) ? (pix8_t)in_bytes[y][x+1] : (pix8_t)0;  // FIXED: cast to pix8_t
            
            // AXI4-Stream sideband signals
            packet.keep = (x+1 < WIDTH) ? 0x3 : 0x1;  // 2 bytes or 1 byte valid
            packet.strb = (x+1 < WIDTH) ? 0x3 : 0x1;
            packet.user = (x == 0 && y == 0) ? 1 : 0;     // Start of Frame (SOF)
            packet.last = (x >= WIDTH-2) ? 1 : 0;         // End of Line (EOL)
            packet.id   = 0;
            packet.dest = 0;

            in_stream.write(packet);
        }
    }
    
    // Run the DUT
    sobel(in_stream, out_stream);

    // Collect output pixels from AXI4-Stream (2 pixels per transaction)
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; x += 2) {  // CHANGED: increment by 2
            axis16_t packet = out_stream.read();
            out_bytes[y][x] = packet.data.range(7, 0);
            if(x+1 < WIDTH) 
                out_bytes[y][x+1] = packet.data.range(15, 8);
        }
    }
    
    // Write the result image
    if(!write_ppm(OUTPUT_PATH, out_bytes)) {
        std::cout << "Failed to write output" << std::endl;
        return -1;
    }

    std::cout << ".... Finishing test ...." << std::endl;

    return 0;
}