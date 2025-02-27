#pragma once

#include "common.hpp"
#include "jpg/utils.hpp"
#include "jpg/DHT.hpp"
#include "jpg/DQT.hpp"
#include "jpg/SOF0.hpp"
#include "jpg/SOS.hpp"

namespace image {

class JPG {
public:
    static JPG load(const std::filesystem::path& path);
};

}