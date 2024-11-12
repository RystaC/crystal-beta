#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <vector>

#include <glm/glm.hpp>

#include "stb_image.h"
#include "stb_image_write.h"

namespace image {

class Image {
    uint8_t* data_;
    uint32_t width_, height_;

public:
    Image(uint8_t* data, uint32_t width, uint32_t height) noexcept : data_(data), width_(width), height_(height) {}
    ~Image() noexcept {
        stbi_image_free(data_);
    }

    auto data() noexcept { return data_; }
    const auto data() const noexcept { return data_; }

    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }

    static Image load(const std::filesystem::path& src_path) {
        int32_t width{}, height{}, stride{};
        uint8_t* data{};

        data = reinterpret_cast<uint8_t*>(stbi_load(src_path.string().c_str(), &width, &height, &stride, 4));
        if(!data) {
            throw std::runtime_error(std::format("[image::Image::load()] ERROR: failed to read file: {}", src_path));
        }

        return Image(data, width, height);
    }

    static void save_png_rgba(const std::filesystem::path& dst_path, const uint8_t* data, uint8_t width, uint8_t height) {
        stbi_write_png(dst_path.string().c_str(), width, height, 4, reinterpret_cast<const void*>(data), 0);
    }
};

}