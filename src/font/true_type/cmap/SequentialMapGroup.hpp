#pragma once

#include "../utils.hpp"

namespace font {

namespace true_type {

namespace cmap {

struct SequentialMapGroup {
    uint32_t start_char_code;
    uint32_t end_char_code;
    uint32_t start_glyph_id;
};

inline SequentialMapGroup read_smg(std::ifstream& ifs) {
    SequentialMapGroup smg{};

    read_be(ifs, smg.start_char_code);
    read_be(ifs, smg.end_char_code);
    read_be(ifs, smg.start_glyph_id);

    return smg;
}

}

}

}