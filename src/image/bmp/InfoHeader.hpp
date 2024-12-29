#pragma once

#include "../common.hpp"

namespace image {

namespace bmp {

struct InfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t plane_count;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels;
    uint32_t y_pixels;
    uint32_t palette_count;
    uint32_t important_palette_index;
};

inline InfoHeader read_info_header(std::ifstream& ifs) {
    InfoHeader header{};

    ifs.read(reinterpret_cast<char*>(&header.size), sizeof(uint32_t));
    if(header.size == 12) {
        ifs.read(reinterpret_cast<char*>(&header.width), sizeof(int16_t));
        ifs.read(reinterpret_cast<char*>(&header.height), sizeof(int16_t));
        ifs.read(reinterpret_cast<char*>(&header.plane_count), sizeof(uint16_t));
        ifs.read(reinterpret_cast<char*>(&header.bit_count), sizeof(uint16_t));

        return header;
    }
    else {
        ifs.read(reinterpret_cast<char*>(&header.width), sizeof(int32_t));
        ifs.read(reinterpret_cast<char*>(&header.height), sizeof(int32_t));
        ifs.read(reinterpret_cast<char*>(&header.plane_count), sizeof(uint16_t));
        ifs.read(reinterpret_cast<char*>(&header.bit_count), sizeof(uint16_t));
        ifs.read(reinterpret_cast<char*>(&header.compression), sizeof(uint32_t));
        ifs.read(reinterpret_cast<char*>(&header.image_size), sizeof(uint32_t));
        ifs.read(reinterpret_cast<char*>(&header.x_pixels), sizeof(uint32_t));
        ifs.read(reinterpret_cast<char*>(&header.y_pixels), sizeof(uint32_t));
        ifs.read(reinterpret_cast<char*>(&header.palette_count), sizeof(uint32_t));
        ifs.read(reinterpret_cast<char*>(&header.important_palette_index), sizeof(uint32_t));

        return header;
    }
}

}

}