#pragma once

#include <algorithm>
#include <format>
#include <memory>

#include "../vkw/_Instance.hpp"
#include "../vkw/_Device.hpp"
#include "../vkw/_PhysicalDevice.hpp"
#include "../vkw/_Queue.hpp"
#include "../vkw/_CommandBuffer.hpp"

namespace graphics {

template<typename T>
inline uint32_t size_u32(const T& sized) {
    return static_cast<uint32_t>(sized.size());
}

}