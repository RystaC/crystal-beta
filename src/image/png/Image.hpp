#pragma once

#include "utils.hpp"

namespace image {

namespace png {

struct Image {
    uint32_t width;
    uint32_t height;
    uint32_t component_count;
    uint32_t index;
    std::vector<uint8_t> filters;
    std::vector<uint8_t> data;

    Image(uint32_t w, uint32_t h, uint32_t c) : width(w), height(h), component_count(c), index(0) {
        filters.reserve(height);
        data.reserve(width * height * component_count);
    }

    auto size() const noexcept { return index; }

    void add_component(uint8_t value) {
        if(index % (width * component_count + 1) == 0) {
            filters.push_back(value);
            ++index;
        }
        else {
            data.push_back(value);
            ++index;
        }
    }

    uint8_t get_component(size_t i) {
        auto row = i / (width * component_count + 1);
        auto col = i % (width * component_count + 1);
        if(col == 0) {
            return filters[row];
        }
        else {
            return data[row * (width * component_count) + col - 1];
        }
    }
};

}

}