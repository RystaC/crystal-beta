#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

inline std::filesystem::path read_text(std::ifstream& ifs, bool is_utf8 = false) {
    uint32_t byte_size{};
    ifs.read(reinterpret_cast<char*>(&byte_size), sizeof(uint32_t));
    std::vector<uint8_t> bytes(byte_size);
    ifs.read(reinterpret_cast<char*>(bytes.data()), sizeof(uint8_t) * bytes.size());
    if(is_utf8) {
        return std::filesystem::path(reinterpret_cast<char8_t*>(bytes.data()), reinterpret_cast<char8_t*>(bytes.data()) + byte_size);
    }
    else {
        return std::filesystem::path(reinterpret_cast<char16_t*>(bytes.data()), reinterpret_cast<char16_t*>(bytes.data()) + byte_size / 2);
    }
}

inline int32_t read_index(std::ifstream& ifs, uint8_t index_size) {
    if(index_size == 1) {
        int8_t index{};
        ifs.read(reinterpret_cast<char*>(&index), index_size);
        return index;
    }
    else if(index_size == 2) {
        int16_t index{};
        ifs.read(reinterpret_cast<char*>(&index), index_size);
        return index;
    }
    else if(index_size == 4) {
        int32_t index{};
        ifs.read(reinterpret_cast<char*>(&index), index_size);
        return index;
    }
    else {
        return -1;
    }
}

}

}