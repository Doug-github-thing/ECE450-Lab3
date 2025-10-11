#include <cstdio>
#include <iostream>
#include <ap_int.h>
#include <array>
#include <fstream>
#include <memory>

#define INPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_in_greyscale.ppm"
#define OUTPUT_PATH "/home/dog/school/fpgaece450/p3/sobel_out_tb.ppm"
#define WIDTH 512
#define HEIGHT 512

void sobel (ap_int<8> in_bytes[WIDTH][HEIGHT], ap_int<8> out_bytes[WIDTH][HEIGHT]);


// Simple PPM reader/writer
bool read_ppm(const char* filename, ap_int<8> img[HEIGHT][WIDTH]) {
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


bool write_ppm(const char* filename, ap_int<8> img[HEIGHT][WIDTH]) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;

    file << "P5 " << WIDTH << " " << HEIGHT << " 255\n";
    file.write(reinterpret_cast<char*>(img), WIDTH * HEIGHT);
    return true;
}


int main() {


    std::cout << "!!!! Starting test !!!!" << std::endl;
    
    ap_int<8> in_bytes[512][512];
    ap_int<8> out_bytes[512][512];
    
    if(!read_ppm(INPUT_PATH, in_bytes))
        return -1;

    sobel(in_bytes, out_bytes);
    
    if(!write_ppm(OUTPUT_PATH, out_bytes))
        return -1;

    std::cout << ".... Finshing test ...." << std::endl;

    return 0;
}
