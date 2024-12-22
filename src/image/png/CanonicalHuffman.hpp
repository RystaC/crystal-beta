#pragma once

#include "utils.hpp"
#include "Deflate.hpp"

namespace image {

namespace png {

struct CanonicalHuffman {
    std::vector<std::pair<uint16_t, uint16_t>> code_lengths;
    std::vector<uint8_t> code_counts;

    uint16_t read_code(const Deflate& bit_stream, size_t& index) {
        uint16_t code{};
        uint16_t first_code{};
        uint16_t first_index{};
        
        for(size_t i = 0; i < code_counts.size(); ++i) {
            code |= bit_stream.read_bits(1, index);
            // std::cerr << std::format("code = {:b}, first code = {:b}, first index = {}, code - first code = {}, code counts[{}] = {}", code, first_code, first_index, code - first_code, i, code_counts[i]) << std::endl;
            if(code - first_code < code_counts[i]) {
                return code_lengths[code - first_code + first_index].first;
            }
            code <<= 1;
            first_code += code_counts[i];
            first_code <<= 1;
            first_index += code_counts[i];
        }

        return code_lengths[code - first_code + first_index].first;
    }

    void print() const noexcept {
        for(size_t i = 0; i < code_lengths.size(); ++i) {
            std::cerr << std::format("#{}: value = {}, length = {}", i, code_lengths[i].first, code_lengths[i].second) << std::endl;
        }
        for(size_t i = 0; i < code_counts.size(); ++i) {
            std::cerr << std::format("length = {}, count = {}", i + 1, code_counts[i]) << std::endl;
        }
    }
};

}

}