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

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/glm.hpp>

#include "debug.hpp"

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

struct Resolution {
    uint32_t width, height;
};

template<typename T>
class Result {
    T object_;
    VkResult result_;

public:
    Result(T&& object, VkResult result) noexcept : object_(std::move(object)), result_(result) {}

    operator bool() const noexcept { return result_ >= VK_SUCCESS; }
    bool is_success() const noexcept { return result_ == VK_SUCCESS; }
    bool is_error() const noexcept { return result_ < VK_SUCCESS; }

    T unwrap() {
        if(result_ >= VK_SUCCESS) return std::move(object_);
        else {
            throw std::runtime_error(std::format("[vkw] ERROR: error code is returned. code: {}", result_to_str(result_)));
        }
    }
};

}