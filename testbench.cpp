#include <cstdio>
#include <iostream>
#include <ap_int.h>
#include <fstream>

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


bool verify_output(ap_int<8> input[HEIGHT][WIDTH], ap_int<8> filter_output[HEIGHT][WIDTH]) {
    ap_int<8> expected_output[HEIGHT][WIDTH];

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
                    Gx += in[y+i][x+i] * Kx[1+i][1+j];
                    Gy += in[y+i][x+i] * Ky[1+i][1+j];
                }
            
            // Hypotenuse between x and y axis
            int G = sqrtf(Gx*Gx + Gy*Gy);

            // Normalize back to 255 range
            out[y][x] = (G  * 255 / 1443);
        }
    }
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
