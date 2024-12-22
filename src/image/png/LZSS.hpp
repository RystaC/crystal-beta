#pragma once

#include "utils.hpp"

namespace image {

namespace png {

struct LZSS {
    static constexpr std::array<std::pair<uint16_t, uint8_t>, 29> MATCH_TABLE = {
        std::make_pair(3, 0),
        std::make_pair(4, 0),
        std::make_pair(5, 0),
        std::make_pair(6, 0),
        std::make_pair(7, 0),
        std::make_pair(8, 0),
        std::make_pair(9, 0),
        std::make_pair(10, 0),
        std::make_pair(11, 1),
        std::make_pair(13, 1),
        std::make_pair(15, 1),
        std::make_pair(17, 1),
        std::make_pair(19, 2),
        std::make_pair(23, 2),
        std::make_pair(27, 2),
        std::make_pair(31, 2),
        std::make_pair(35, 3),
        std::make_pair(43, 3),
        std::make_pair(51, 3),
        std::make_pair(59, 3),
        std::make_pair(67, 4),
        std::make_pair(83, 4),
        std::make_pair(99, 4),
        std::make_pair(115, 4),
        std::make_pair(131, 5),
        std::make_pair(163, 5),
        std::make_pair(195, 5),
        std::make_pair(227, 5),
        std::make_pair(258, 0),
    };

    static constexpr std::array<std::pair<uint16_t, uint8_t>, 30> DISTANCE_TABLE = {
        std::make_pair(1, 0),
        std::make_pair(2, 0),
        std::make_pair(3, 0),
        std::make_pair(4, 0),
        std::make_pair(5, 1),
        std::make_pair(7, 1),
        std::make_pair(9, 2),
        std::make_pair(13, 2),
        std::make_pair(17, 3),
        std::make_pair(25, 3),
        std::make_pair(33, 4),
        std::make_pair(49, 4),
        std::make_pair(65, 5),
        std::make_pair(97, 5),
        std::make_pair(129, 6),
        std::make_pair(193, 6),
        std::make_pair(257, 7),
        std::make_pair(385, 7),
        std::make_pair(513, 8),
        std::make_pair(769, 8),
        std::make_pair(1025, 9),
        std::make_pair(1537, 9),
        std::make_pair(2049, 10),
        std::make_pair(3073, 10),
        std::make_pair(4097, 11),
        std::make_pair(6145, 11),
        std::make_pair(8193, 12),
        std::make_pair(12289, 12),
        std::make_pair(16385, 13),
        std::make_pair(24577, 13),
    };
};

}

}