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
};

struct VertexAttribute {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
    glm::vec4 color;
};

}