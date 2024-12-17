#pragma once

#include "../utils.hpp"

namespace font {

namespace true_type {

namespace cmap {

struct Subtable {
    uint16_t platform_id;
    uint16_t platform_specific_id;
    uint32_t offset;
};

inline Subtable read_subtable(std::ifstream& ifs) {
    Subtable subtable{};

    read_be(ifs, subtable.platform_id);
    read_be(ifs, subtable.platform_specific_id);
    read_be(ifs, subtable.offset);

    return subtable;
}

}

}

}