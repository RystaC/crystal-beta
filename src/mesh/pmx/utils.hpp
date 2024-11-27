#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

inline std::filesystem::path read_text(std::istream& ifs, bool is_utf8 = false) {
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

}

}