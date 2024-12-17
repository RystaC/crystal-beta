#pragma once

#include "../common.hpp"

namespace font {

namespace true_type {

using ShortFrac = int16_t;
using Fixed = int32_t;
using FWord = int16_t;
using UFWord = uint16_t;
using F2Dot14 = int16_t;
using LongDateTime = int64_t;

template<typename T>
void read_be(std::ifstream& ifs, T& dst) {
    if constexpr (sizeof(T) == 1) {
        uint8_t bytes{};
        ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));
        dst = std::bit_cast<T, uint8_t>(bytes);
    }
    else if constexpr(sizeof(T) == 2) {
        uint16_t bytes{};
        ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

        bytes = (((bytes >> 000) & 0xff) << 010) |
                (((bytes >> 010) & 0xff) << 000);

        dst = std::bit_cast<T, uint16_t>(bytes);
    }
    else if constexpr(sizeof(T) == 4) {
        uint32_t bytes{};
        ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

        bytes = (((bytes >> 000) & 0xff) << 030) |
                (((bytes >> 010) & 0xff) << 020) |
                (((bytes >> 020) & 0xff) << 010) |
                (((bytes >> 030) & 0xff) << 000);

        dst = std::bit_cast<T, uint32_t>(bytes);
    }
    else if constexpr(sizeof(T) == 8) {
        uint64_t bytes{};
        ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

        bytes = (((bytes >> 000) & 0xff) << 070) |
                (((bytes >> 010) & 0xff) << 060) |
                (((bytes >> 020) & 0xff) << 050) |
                (((bytes >> 030) & 0xff) << 040) |
                (((bytes >> 040) & 0xff) << 030) |
                (((bytes >> 050) & 0xff) << 020) |
                (((bytes >> 060) & 0xff) << 010) |
                (((bytes >> 070) & 0xff) << 000);

        dst = std::bit_cast<T, uint64_t>(bytes);
    }
    else {
        static_assert([]{ return false; }, "[font::TrueType::read_be_] ERROR: unexpeced byte size");
    }
}

}

}