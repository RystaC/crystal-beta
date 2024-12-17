#pragma once

#include "utils.hpp"

namespace font {

namespace true_type {

struct Loca {
    std::vector<uint32_t> offsets;
};

inline Loca read_loca_short(std::ifstream& ifs, uint16_t num_glyphs) {
    Loca loca{};
    loca.offsets.resize(num_glyphs);

    for(size_t i = 0; i < loca.offsets.size(); ++i) {
        uint16_t offset{};
        read_be(ifs, offset);
        loca.offsets[i] = offset;
    }

    return loca;
}

inline Loca read_loca_long(std::ifstream& ifs, uint16_t num_glyphs) {
    Loca loca{};
    loca.offsets.resize(num_glyphs);

    for(size_t i = 0; i < loca.offsets.size(); ++i) {
        uint32_t offset{};
        read_be(ifs, offset);
        loca.offsets[i] = offset;
    }

    return loca;
}

}

}