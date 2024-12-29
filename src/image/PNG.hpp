#pragma once

#include "common.hpp"
#include "png/IHDR.hpp"
#include "png/IDAT.hpp"
#include "png/Deflate.hpp"
#include "png/LZSS.hpp"
#include "png/FixedHuffman.hpp"
#include "png/CustomHuffman.hpp"
#include "png/Image.hpp"

namespace image {

class PNG {
    uint32_t width_;
    uint32_t height_;
    uint32_t component_count_;
    std::vector<uint8_t> data_;

    static uint8_t paeth_predictor_(uint8_t a, uint8_t b, uint8_t c) {
        auto p = a + b - c;
        auto pa = std::abs(p - a);
        auto pb = std::abs(p - b);
        auto pc = std::abs(p - c);
        if(pa <= pb && pa <= pc) {
            return a;
        }
        else if(pb <= pc) {
            return b;
        }
        else {
            return c;
        }
    }

    PNG(std::vector<uint8_t>&& data, uint32_t width, uint32_t height, uint32_t component_count) noexcept :
        width_(width) , height_(height), component_count_(component_count), data_(data) {}

public:
    PNG() noexcept = default;
    
    static PNG load(const std::filesystem::path& path);

    const auto& data() const noexcept { return data_; }
    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }
    auto component_count() const noexcept { return component_count_; }
};

}