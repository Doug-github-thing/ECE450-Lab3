#include <cstdio>
#include <iostream>
#include <ap_int.h>
#include <array>
#include <fstream>
#include <memory>
// #include "sobel.cpp"

#define INPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_in.ppm"
#define OUTPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_out_tb.ppm"
#define WIDTH 512
#define HEIGHT 512

void sobel (ap_int<8> in_bytes[WIDTH][HEIGHT], ap_int<8> out_bytes[WIDTH][HEIGHT]);


// Simple P5 (binary) PPM reader/writer for grayscale images
bool read_ppm(const char* filename, uint8_t img[HEIGHT][WIDTH]) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "File does not exist" << std::endl;
        return false;
    }

    std::string header;
    int w, h, maxval;
    file >> header >> w >> h >> maxval;
    file.ignore(1); // skip newline
    if (header != "P6" || w != WIDTH || h != HEIGHT || maxval != 255) {
        std::cout << "Incorrect file header" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(img), WIDTH * HEIGHT);
    return true;
}


int main() {


    std::cout << "!!!! Starting test !!!!" << std::endl;
    
    uint8_t in_bytes[512][512];
    uint8_t out_bytes[512][512];
    
    if(!read_ppm(INPUT_PATH, in_bytes))
        return -1;

    std::cout << ".... Finshing test ...." << std::endl;

    return 0;
}
