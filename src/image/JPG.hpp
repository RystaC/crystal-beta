#pragma once

#include "common.hpp"
#include "jpg/utils.hpp"

namespace image {

class JPG {
public:
    static JPG load(const std::filesystem::path& path);
};

}