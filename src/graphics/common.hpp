#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <format>
#include <memory>
#include <limits>

#include "../vkw/Instance.hpp"
#include "../vkw/Device.hpp"
#include "../vkw/PhysicalDevice.hpp"
#include "../vkw/Queue.hpp"
#include "../vkw/CommandBuffer.hpp"

namespace graphics {

template<typename T>
inline uint32_t size_u32(const T& sized) {
    return static_cast<uint32_t>(sized.size());
}

// for debugging
constexpr const char* BYTE_STRS[] = {
    "B",
    "KB",
    "MB",
    "GB",
    "TB",
};

inline std::string byte_to_str(size_t bytes) {
    float value = static_cast<float>(bytes);
    for(size_t i = 0; i < std::size(BYTE_STRS); ++i) {
        if(value < 1024.0f) {
            return std::format("{:.2f} {}", value, BYTE_STRS[i]);
        }

        value /= 1024.0f;
    }

    return std::format("{:.2f} {}", value, BYTE_STRS[4]);
}

constexpr uint8_t U8_MAX = std::numeric_limits<uint8_t>::max();
constexpr uint16_t U16_MAX = std::numeric_limits<uint16_t>::max();
constexpr uint32_t U32_MAX = std::numeric_limits<uint32_t>::max();
constexpr uint64_t U64_MAX = std::numeric_limits<uint64_t>::max();

constexpr int8_t I8_MAX = std::numeric_limits<int8_t>::max();
constexpr int16_t I16_MAX = std::numeric_limits<int16_t>::max();
constexpr int32_t I32_MAX = std::numeric_limits<int32_t>::max();
constexpr int64_t I64_MAX = std::numeric_limits<int64_t>::max();

constexpr int8_t I8_MIN = std::numeric_limits<int8_t>::min();
constexpr int16_t I16_MIN = std::numeric_limits<int16_t>::min();
constexpr int32_t I32_MIN = std::numeric_limits<int32_t>::min();
constexpr int64_t I64_MIN = std::numeric_limits<int64_t>::min();

constexpr float F32_MAX = std::numeric_limits<float>::max();
constexpr double F64_MAX = std::numeric_limits<double>::max();

constexpr float F32_MIN = std::numeric_limits<float>::lowest();
constexpr double F64_MIN = std::numeric_limits<double>::lowest();

namespace literals {

inline size_t operator""_kB(size_t value) {
    return value * 1024;
}
inline size_t operator""_mB(size_t value) {
    return operator""_kB(value) * 1024;
}
inline size_t operator""_gB(size_t value) {
    return operator""_mB(value) * 1024;
}

}

}