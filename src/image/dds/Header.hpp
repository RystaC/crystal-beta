#pragma once

#include "../common.hpp"

namespace image {

namespace dds {

struct Header {
    uint32_t magic;
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitch_or_linear_size;
    uint32_t depth;
    uint32_t mipmap_count;
    uint32_t pf_size;
    uint32_t pf_flags;
    uint32_t four_cc;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
    uint32_t caps;
    uint32_t caps2;
};

inline Header read_header(std::ifstream& ifs) {
    Header header{};

    ifs.read(reinterpret_cast<char*>(&header.magic), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.size), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.flags), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.height), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.width), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.pitch_or_linear_size), sizeof(uint32_t));

    ifs.read(reinterpret_cast<char*>(&header.depth), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.mipmap_count), sizeof(uint32_t));
    ifs.seekg(sizeof(uint32_t) * 11, std::ios::cur);
    ifs.read(reinterpret_cast<char*>(&header.pf_size), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.pf_flags), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.four_cc), sizeof(uint32_t));

    ifs.read(reinterpret_cast<char*>(&header.rgb_bit_count), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.r_bit_mask), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.g_bit_mask), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.b_bit_mask), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.a_bit_mask), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.caps), sizeof(uint32_t));
    ifs.read(reinterpret_cast<char*>(&header.caps2), sizeof(uint32_t));
    ifs.seekg(sizeof(uint32_t) * 3, std::ios::cur);

    return header;
}

}

}