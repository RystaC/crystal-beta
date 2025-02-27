#pragma once

#include "../common.hpp"

namespace image {

namespace jpg {

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

struct BitStream {
    std::vector<uint8_t> data;

    auto size() const noexcept { return data.size() * 8; }

    bool operator[](size_t i) const noexcept { return data[i / 8] & (0x01 << (i % 8)); }

    uint32_t read_bits(size_t bit_count, size_t& index) const noexcept {
        uint32_t value = 0;
        for(size_t i = 0; i < bit_count; ++i) {
            value |= int(this->operator[](index++)) << i;
        }

        return value;
    }
};

constexpr std::array<uint8_t, 64> zigzag_index = {
    0, 1, 5, 6, 14, 15, 27, 28,
    2, 4, 7, 13, 16, 26, 29, 42,
    3, 8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63,
};

}

}