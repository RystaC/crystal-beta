#pragma once

#include "SequentialMapGroup.hpp"

namespace font {

namespace true_type {

namespace cmap {

struct Format12 {
    uint16_t format;
    uint16_t reserved;
    uint32_t length;
    uint32_t language;
    std::vector<SequentialMapGroup> groups;

    uint32_t glyph_index(uint32_t char_code) {
        for(const auto& g : groups) {
            if(g.start_char_code <= char_code && char_code <= g.end_char_code) {
                return g.start_glyph_id + (char_code - g.start_char_code);
            }
        }

        return 0;
    }
};

inline Format12 read_format12(std::ifstream& ifs) {
    Format12 format{};

    format.format = 12;
    read_be(ifs, format.reserved);
    read_be(ifs, format.length);
    read_be(ifs, format.language);
    uint32_t group_count{};
    read_be(ifs, group_count);
    format.groups.resize(group_count);
    for(size_t i = 0; i < format.groups.size(); ++i) {
        format.groups[i] = std::move(read_smg(ifs));
    }

    return format;
}

}

}

}