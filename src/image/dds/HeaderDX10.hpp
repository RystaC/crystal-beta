#pragma once

#include "../common.hpp"

namespace image {

namespace dds {

struct HeaderDX10 {
    uint32_t format;
    uint32_t dimension;
    uint32_t misc_flag;
    uint32_t array_size;
    uint32_t misc_flag2;
};

inline HeaderDX10 read_header_dx10(std::ifstream& ifs) {
    HeaderDX10 header{};

    ifs.read(reinterpret_cast<char*>(&header.format), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.dimension), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.misc_flag), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.array_size), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.misc_flag2), sizeof(uint32_t));

    return header;
}

}

}