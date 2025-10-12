#include "sobel.h"
#include <cstdio>
#include <iostream>
#include <ap_int.h>
#include <fstream>

void sobel (uint8_t in_bytes[WIDTH][HEIGHT], uint8_t out_bytes[WIDTH][HEIGHT]);


// Simple PPM reader/writer
bool read_ppm(const char* filename, uint8_t img[HEIGHT][WIDTH]) {
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


bool write_ppm(const char* filename, uint8_t img[HEIGHT][WIDTH]) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file << "P5 " << WIDTH << " " << HEIGHT << " 255\n";
    file.write(reinterpret_cast<char*>(img), WIDTH * HEIGHT);
    return true;
}


bool verify_output(uint8_t in[HEIGHT][WIDTH], uint8_t filter_output[HEIGHT][WIDTH]) {

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
            int G = sqrtf(Gx*Gx + Gy*Gy);

            // Normalize back to 255 range, clamping max values at a chosen value
            if (G >= NORMALIZATION_FACTOR)
                G = 255;
            G = (G  * 255 / NORMALIZATION_FACTOR);
            uint8_t this_element = G;

            // Element-wise check the filter result against the expected values
            if (this_element != filter_output[y][x]) {
                std::cout << "Inconsistency at " << x << ", " << y << std::endl;
                std::cout << "Expected: " << this_element << ". Actual: " << filter_output[y][x] << std::endl;
                return false;
            }
        }
    }

    std::cout << "Verification successful" << std::endl;
    return true;
}


int main() {


    std::cout << "!!!! Starting test !!!!" << std::endl;
    
    uint8_t in_bytes[512][512];
    uint8_t out_bytes[512][512];
    
    if(!read_ppm(INPUT_PATH, in_bytes))
        return -1;

    sobel(in_bytes, out_bytes);
    
    if(!write_ppm(OUTPUT_PATH, out_bytes))
        return -1;

    if (!verify_output(in_bytes, out_bytes))
        return -1;

    std::cout << ".... Finshing test ...." << std::endl;

    return 0;
}
