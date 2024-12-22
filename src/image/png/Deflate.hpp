#pragma once

#include "utils.hpp"
#include "IDAT.hpp"

namespace image {

namespace png {

struct Deflate {
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

inline Deflate concat_idat(std::vector<IDAT>&& data_chunks, size_t total_length) {
    Deflate deflate{};
    deflate.data.resize(total_length);

    size_t current_index = 0;
    for(size_t i = 0; i < data_chunks.size(); ++i) {
        for(size_t j = 0; j < data_chunks[i].data.size(); ++j) {
            deflate.data[current_index++] = data_chunks[i].data[j];
        }
    }

    return deflate;
}

}

}