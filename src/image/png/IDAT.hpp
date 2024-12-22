#pragma once

#include "utils.hpp"

namespace image {

namespace png {

struct IDAT {
    std::vector<uint8_t> data;
};

inline IDAT read_idat(std::ifstream& ifs, uint32_t length) {
    IDAT idat{};
    idat.data.resize(length);
    ifs.read(reinterpret_cast<char*>(idat.data.data()), sizeof(uint8_t) * length);

    return idat;
}

}

}