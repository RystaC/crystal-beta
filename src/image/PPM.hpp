#pragma once

#include "common.hpp"

namespace image {

class PPM {
    uint32_t width_;
    uint32_t height_;
    uint32_t component_count_;
    std::vector<uint8_t> data_;

    PPM(std::vector<uint8_t>&& data, uint32_t width, uint32_t height, uint32_t component_count) noexcept :
        width_(width), height_(height), component_count_(component_count), data_(data) {}

public:
    PPM() noexcept = default;

    static PPM load(const std::filesystem::path& path);

    const auto& data() const noexcept { return data_; }
    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }
    auto component_count() const noexcept { return component_count_; }
};

}