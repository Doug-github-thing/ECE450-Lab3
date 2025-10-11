#include <cstdio>
#include <iostream>
#include <ap_int.h>
#include <array>
#include <fstream>
#include <memory>

#define input_path "sobel_in.ppm"
#define output_path "sobel_out_tb.ppm"

void sobel (ap_int<8> in_bytes[512][512], ap_int<8> out_bytes[512][512]);

int main() {

    // std::ifstream in( input_path, std::ios::binary );
    // std::ofstream out( output_path, std::ios::binary );

    FILE* in = fopen(input_path, "rb");
    unsigned char* buffer = new unsigned char[14+512*512];
    fread(buffer, 14+512*512, 14+512*512, in);

    ap_int<8> in_bytes[512][512];
    ap_int<8> out_bytes[512][512];

    // for (int i=0; i<512; ++i)
    //     for (int j=0; j<512; ++j)
    //         in_bytes[i][j] = in.read();

    // in.seekg( 0, std::ios::end );
    // auto size = in.tellg();
    // in.seekg( 0 );

    // char buffer[512];

    // std::cout << "Gonna read bytes now" << std::endl;
    // in.read( buffer, 512 );
    std::cout << "Got bytes. Gonna print them now" << std::endl;

    for (int i=0; i<512*512; ++i)
        std::cout << buffer[i];
    delete[] buffer;

    // out.write( buffer.get(), size );

    std::cout << "Done testing" << std::endl;

    fclose(in);
    // out.close();

    return 0;
}
