#pragma once

#include "common.hpp"
#include "dds/Header.hpp"
#include "dds/HeaderDX10.hpp"

namespace image {

class DDS {
    uint32_t width_;
    uint32_t height_;
    uint32_t component_count_;
    uint8_t compression_type_;
    std::vector<uint8_t> data_;

    static uint32_t make_four_cc_(const char* key) {
        return (key[3] << 24) | (key[2] << 16) | (key[1] << 8) | (key[0]);
    }

    DDS(std::vector<uint8_t>&& data, uint32_t width, uint32_t height) noexcept :
        width_(width), height_(height), data_(data) {}

public:
    DDS() noexcept = default;

    static DDS load(const std::filesystem::path& path);

    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }

    const auto& data() const noexcept { return data_; }
};

}