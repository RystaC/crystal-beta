#pragma once

#include "utils.hpp"

namespace image {

namespace png {

struct IHDR {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

inline IHDR read_ihdr(std::ifstream& ifs) {
    IHDR ihdr{};

    read_be(ifs, ihdr.width);
    read_be(ifs, ihdr.height);
    read_be(ifs, ihdr.bit_depth);
    read_be(ifs, ihdr.color_type);
    read_be(ifs, ihdr.compression_method);
    read_be(ifs, ihdr.filter_method);
    read_be(ifs, ihdr.interlace_method);

    return ihdr;
}

}

}