#pragma once

#include "utils.hpp"

namespace font {

namespace true_type {

struct Head {
    Fixed version;
    Fixed font_revision;
    uint32_t check_sum_adjustment;
    uint32_t magic_number;
    uint16_t flags;
    uint16_t units_per_em;
    LongDateTime created;
    LongDateTime modified;
    FWord x_min, y_min, x_max, y_max;
    uint16_t mac_style;
    uint16_t lowest_rect_ppem;
    int16_t font_direction_hint;
    int16_t index_to_loc_format;
    int16_t glyph_data_format;
};

inline Head read_head(std::ifstream& ifs) {
    Head head{};
    read_be(ifs, head.version);
    read_be(ifs, head.font_revision);
    read_be(ifs, head.check_sum_adjustment);
    read_be(ifs, head.magic_number);
    read_be(ifs, head.flags);
    read_be(ifs, head.units_per_em);
    read_be(ifs, head.created);
    read_be(ifs, head.modified);
    read_be(ifs, head.x_min);
    read_be(ifs, head.y_min);
    read_be(ifs, head.x_max);
    read_be(ifs, head.y_max);
    read_be(ifs, head.mac_style);
    read_be(ifs, head.lowest_rect_ppem);
    read_be(ifs, head.font_direction_hint);
    read_be(ifs, head.index_to_loc_format);
    read_be(ifs, head.glyph_data_format);

    return head;
}

}

}