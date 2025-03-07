#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <cstring>

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/glm.hpp>

namespace vkw {

template<typename T>
concept Sized = requires(const T& v) {
    v.size();
};

// utility functions
template<Sized T>
inline uint32_t size_u32(const T& v) {
    return static_cast<uint32_t>(v.size());
}

}