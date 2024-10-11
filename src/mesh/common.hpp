#pragma once

#include <algorithm>
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

struct AABB {
    alignas(16) glm::vec3 min;
    alignas(16) glm::vec3 max;

    float area() const {
        auto d = max - min;
        return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
    }

    glm::vec3 centroid() const {
        return (min + max) / 2.0f;
    }
};

inline float aabb_area(glm::vec3 min, glm::vec3 max) {
    auto d = max - min;
    return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
}

struct VertexAttribute {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
    glm::vec4 color;
};

}