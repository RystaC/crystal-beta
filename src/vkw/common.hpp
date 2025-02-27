#pragma once

#include <algorithm>
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

// utility functions
template<typename T>
inline uint32_t size_u32(T sized) {
    return static_cast<uint32_t>(s.size());
}

}