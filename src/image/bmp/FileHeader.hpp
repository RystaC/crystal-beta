#pragma once

#include "../common.hpp"

namespace image {

namespace bmp {

struct FileHeader {
    std::string type;
    uint32_t size;
    uint32_t offset;
};

inline FileHeader read_file_header(std::ifstream& ifs) {
    FileHeader header{};
    header.type.resize(2, '\0');

    ifs.read(reinterpret_cast<char*>(header.type.data()), sizeof(char) * 2);
    ifs.read(reinterpret_cast<char*>(&header.size), sizeof(uint32_t));
    ifs.seekg(2 * 2, std::ios::cur);
    ifs.read(reinterpret_cast<char*>(&header.offset), sizeof(uint32_t));

    return header;
}

}

}