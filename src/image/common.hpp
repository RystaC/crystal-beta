#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace image {
    enum class Format {
        RGB8,
        RGBA8,
        RGB16,
        RGBA16,
        BC1,
        BC2,
        BC3,
        BC4U,
        BC4S,
        BC5U,
        BC5S,
        BC6HU,
        BC6HS,
        BC7,
    };
}