#pragma once

#include "utils.hpp"
#include "CanonicalHuffman.hpp"

namespace image {

namespace png {

struct CustomHuffman {
    static constexpr std::array<uint16_t, 19> CODE_LENGTH_CODE_INDICES = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
    };

    uint16_t hlit;
    uint16_t hdist;
    uint16_t hclen;
    CanonicalHuffman code_lengths;
    CanonicalHuffman characters;
    CanonicalHuffman distances;

    void read_header(const Deflate& bit_stream, size_t& index) {
        hlit = bit_stream.read_bits(5, index);
        hlit += 257;

        hdist = bit_stream.read_bits(5, index);
        hdist += 1;

        hclen = bit_stream.read_bits(4, index);
        hclen += 4;
    }

    void read_code_length_codes(const Deflate& bit_stream, size_t& index) {
        std::vector<std::pair<uint16_t, uint16_t>> code_length_code_lengths{};
        std::vector<uint8_t> code_length_code_counts(7);

        for(uint16_t i = 0; i < hclen; ++i) {
            auto length = bit_stream.read_bits(3, index);
            if(length != 0) {
                code_length_code_lengths.push_back({CODE_LENGTH_CODE_INDICES[i], length});
                ++code_length_code_counts[length - 1];
            }
        }

        std::sort(code_length_code_lengths.begin(), code_length_code_lengths.end(),
            [](const auto& a, const auto& b) {
                if(a.second != b.second) {
                    return a.second < b.second;
                }
                else {
                    return a.first < b.first;
                }
            }
        );

        code_lengths = {code_length_code_lengths, code_length_code_counts};
    }

    void read_character_length_codes(const Deflate& bit_stream, size_t& index) {
        std::vector<std::pair<uint16_t, uint16_t>> character_code_lengths{};
        std::vector<uint8_t> character_code_counts(15);
        uint16_t prev_length{};

        for(uint16_t i = 0; i < hlit;) {
            auto result = code_lengths.read_code(bit_stream, index);
            if(result == 16) {
                auto count = bit_stream.read_bits(2, index);
                count += 3;
                for(uint32_t k = 0; k < count; ++k) {
                    if(prev_length != 0) {
                        character_code_lengths.push_back({i, prev_length});
                        ++character_code_counts[prev_length - 1];
                    }
                    ++i;
                }
            }
            else if(result == 17) {
                auto count = bit_stream.read_bits(3, index);
                count += 3;
                i += count;
            }
            else if(result == 18) {
                auto count = bit_stream.read_bits(7, index);
                count += 11;
                i += count;
            }
            else {
                if(result != 0) {
                    character_code_lengths.push_back({i, result});
                    ++character_code_counts[result - 1];
                }
                prev_length = result;
                ++i;
            }
        }

        std::sort(character_code_lengths.begin(), character_code_lengths.end(),
            [](const auto& a, const auto& b) {
                if(a.second != b.second) {
                    return a.second < b.second;
                }
                else {
                    return a.first < b.first;
                }
            }
        );

        characters = {character_code_lengths, character_code_counts};
    }

    void read_distance_length_codes(const Deflate& bit_stream, size_t& index) {
        std::vector<std::pair<uint16_t, uint16_t>> distance_code_lengths{};
        std::vector<uint8_t> distance_code_counts(15);
        uint16_t prev_length{};

        for(uint16_t i = 0; i < hdist;) {
            auto result = code_lengths.read_code(bit_stream, index);
            if(result == 16) {
                auto count = bit_stream.read_bits(2, index);
                count += 3;
                for(uint32_t k = 0; k < count; ++k) {
                    if(prev_length != 0) {
                        distance_code_lengths.push_back({i, prev_length});
                        ++distance_code_counts[prev_length - 1];
                    }
                    ++i;
                }
            }
            else if(result == 17) {
                auto count = bit_stream.read_bits(3, index);
                count += 3;
                i += count;
            }
            else if(result == 18) {
                auto count = bit_stream.read_bits(7, index);
                count += 11;
                i += count;
            }
            else {
                if(result != 0) {
                    distance_code_lengths.push_back({i, result});
                    ++distance_code_counts[result - 1];
                }
                prev_length = result;
                ++i;
            }
        }

        std::sort(distance_code_lengths.begin(), distance_code_lengths.end(),
            [](const auto& a, const auto& b) {
                if(a.second != b.second) {
                    return a.second < b.second;
                }
                else {
                    return a.first < b.first;
                }
            }
        );

        distances = {distance_code_lengths, distance_code_counts};
    }
};

}

}