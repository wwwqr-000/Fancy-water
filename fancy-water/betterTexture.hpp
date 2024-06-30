#pragma once
#include "point2.hpp"
#include <bardrix/point3.h>
#include "cube.h"
#include "sphere.h"
#include <fstream>
#include <cstdint>
#include <vector>

class betterTexture {
    private:
        std::string imgPath;

        #pragma pack(push, 1)
        struct BMPHeader {
            uint16_t bfType;
            uint32_t bfSize;
            uint16_t bfReserved1;
            uint16_t bfReserved2;
            uint32_t bfOffBits;
        };

        struct DIBHeader {
            uint32_t biSize;
            int32_t biWidth;
            int32_t biHeight;
            uint16_t biPlanes;
            uint16_t biBitCount;
            uint32_t biCompression;
            uint32_t biSizeImage;
            int32_t biXPelsPerMeter;
            int32_t biYPelsPerMeter;
            uint32_t biClrUsed;
            uint32_t biClrImportant;
        };

        #pragma pack(pop)
        struct RGB {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
        };

    public:
        //Minimal init
        betterTexture() {}
        //Fully init
        betterTexture(std::string imgPath) : imgPath(imgPath) {}

        //Getter and setter for BMP
        void setBMP(std::string imgPath) { this->imgPath = imgPath; }
        std::string getBMP() { return this->imgPath; }
        //

        bardrix::point3 getPixelValue(int32_t x, int32_t y) {//Get RGB value from pixel coordinates (Based on BMP format)
            std::ifstream bmp(this->imgPath, std::ios::binary);
            if (!bmp) {
                return bardrix::point3(1.0, 1.0, 1.0);
            }

            BMPHeader bmpHeader;
            bmp.read(reinterpret_cast<char*>(&bmpHeader), sizeof(BMPHeader));
            if (bmpHeader.bfType != 0x4D42) {//Check if file really is a BMP.
                return bardrix::point3(2.0, 2.0, 2.0);
            }

            DIBHeader dibHeader;
            bmp.read(reinterpret_cast<char*>(&dibHeader), sizeof(DIBHeader));
            if (dibHeader.biBitCount != 24) {
                return bardrix::point3(3.0, 3.0, 3.0);
            }

            int32_t width = dibHeader.biWidth;
            int32_t height = dibHeader.biHeight;
            int32_t adjustedY = height - y - 1;//Inverted y-dim. (y=0 was on the bottom of the texture file, now it's the top.)
            int rowSize = (width * 3 + 3) & (~3);
            int pixelOffset = bmpHeader.bfOffBits + adjustedY * rowSize + x * 3;
            bmp.seekg(pixelOffset, std::ios::beg);
            RGB pixel;
            bmp.read(reinterpret_cast<char*>(&pixel), sizeof(RGB));

            return bardrix::point3((double)pixel.red, (double)pixel.green, (double)pixel.blue);
        }
};