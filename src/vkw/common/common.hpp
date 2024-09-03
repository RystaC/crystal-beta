#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

namespace vkw {

// utility macros
#define CHECK_VK_RESULT(result, stmt) if(result != VK_SUCCESS) { stmt }

// utility functions
inline uint32_t size_u32(size_t s) { return static_cast<uint32_t>(s); }

template<typename T, typename U = T::object_type>
inline std::vector<U> detach(const std::vector<T>& wrapper_vector) {
    std::vector<U> dst(wrapper_vector.size());

    std::transform(wrapper_vector.begin(), wrapper_vector.end(), dst.begin(), [](const auto& wrapped) { return static_cast<U>(wrapped); });

    return dst;
}

template<typename T>
struct Transition {
    T src, dst;

    Transition(T s, T d) : src(s), dst(d) {}
};

}