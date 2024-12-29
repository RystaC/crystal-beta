#pragma once

#include "utils.hpp"
#include "CanonicalHuffman.hpp"

namespace image {

namespace png {

struct FixedHuffman {
    CanonicalHuffman characters;
    CanonicalHuffman distances;

    void init() {
        characters.code_lengths.resize(288);
        characters.code_counts.resize(15);
        for(uint16_t i = 0; i < 287; ++i) {
            if(i <= 143) {
                characters.code_lengths[i] = std::make_pair(i, 8);
                ++characters.code_counts[7];
            }
            else if(i <= 255) {
                characters.code_lengths[i] = std::make_pair(i, 9);
                ++characters.code_counts[8];
            }
            else if(i <= 279) {
                characters.code_lengths[i] = std::make_pair(i, 7);
                ++characters.code_counts[6];    
            }
            else {
                characters.code_lengths[i] = std::make_pair(i, 8);
                ++characters.code_counts[7];
            }
        }

        std::sort(characters.code_lengths.begin(), characters.code_lengths.end(),
            [](const auto& a, const auto& b) {
                if(a.second != b.second) {
                    return a.second < b.second;
                }
                else {
                    return a.first < b.first;
                }
            }
        );

        distances.code_lengths.resize(32);
        distances.code_counts.resize(15);
        for(uint16_t i = 0; i < 31; ++i) {
            distances.code_lengths[i] = std::make_pair(i, 5);
            ++distances.code_counts[4];
        }
    }
};

}

}