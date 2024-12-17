#pragma once

#include <algorithm>
#include <bit>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace mesh {

// axis-aligned bounding box (for instance culling)
struct AABB {
    alignas(16) glm::vec3 min;
    alignas(16) glm::vec3 max;

    float area() const {
        auto d = max - min;
        return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
    }

    glm::vec3 center() const {
        return (min + max) / 2.0f;
    }

    glm::vec3 extent() const {
        return glm::abs(max - center());
    }
};

inline float aabb_area(glm::vec3 min, glm::vec3 max) {
    auto d = max - min;
    return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
}

// basic attribute (used in all mesh types)
struct VertexAttribute {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
    glm::vec4 color;
};

// debug utilities (hash color function etc.)
inline glm::vec4 hash_color(uint32_t i) {
    float r = static_cast<float>(i);
    float g = static_cast<float>(i+1);
    float b = static_cast<float>(i+2);

    for(int u = 0; u < 2; ++u) {
        r = std::fmod(87.0f * r + 23.0f * g + 125.0f * b, 257.0f);
        g = std::fmod(87.0f * r + 23.0f * g + 125.0f * b, 1009.0f);
        b = std::fmod(87.0f * r + 23.0f * g + 125.0f * b, 21001.0f);
    }

    return glm::vec4(r / 257.0f, g / 1009.0f, b / 21001.0f, 1.0f);
}

}

// formatter for glm (maybe used by other components)
template<>
struct std::formatter<glm::vec2> : std::formatter<std::string> {
    auto format(glm::vec2 v, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("({}, {})", v.x, v.y), ctx);
    }
};

template<>
struct std::formatter<glm::vec3> : std::formatter<std::string> {
    auto format(glm::vec3 v, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("({}, {}, {})", v.x, v.y, v.z), ctx);
    }
};

template<>
struct std::formatter<glm::vec4> : std::formatter<std::string> {
    auto format(glm::vec4 v, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w), ctx);
    }
};
