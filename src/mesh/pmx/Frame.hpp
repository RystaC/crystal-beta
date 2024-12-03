#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct FrameElement {
    uint8_t target;
    int32_t index;
};

inline FrameElement read_frame_element(std::ifstream& ifs, uint8_t bone_index_size, uint8_t morph_index_size) {
    FrameElement element{};

    // target
    ifs.read(reinterpret_cast<char*>(&element.target), sizeof(uint8_t));
    if(!element.target) {
        // bone index
        element.index = read_index(ifs, bone_index_size);
    }
    else {
        // morph index
        element.index = read_index(ifs, morph_index_size);
    }

    return element;
}

struct Frame {
    std::vector<FrameElement> elements;
    uint8_t is_special;

    // meta data
    std::filesystem::path name;
    std::filesystem::path name_en;
};

inline Frame read_frame(std::ifstream& ifs, uint8_t bone_index_size, uint8_t morph_index_size, bool is_utf8) {
    Frame frame{};

    // name
    frame.name = std::move(read_text(ifs, is_utf8));
    frame.name_en = std::move(read_text(ifs, is_utf8));
    // flag
    ifs.read(reinterpret_cast<char*>(&frame.is_special), sizeof(uint8_t));
    // number of elements
    uint32_t element_count{};
    ifs.read(reinterpret_cast<char*>(&element_count), sizeof(uint32_t));
    frame.elements.resize(element_count);
    for(size_t i = 0; i < frame.elements.size(); ++i) {
        frame.elements[i] = std::move(read_frame_element(ifs, bone_index_size, morph_index_size));
    }

    return frame;
}

}

}