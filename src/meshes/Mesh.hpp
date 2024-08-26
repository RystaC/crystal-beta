#pragma once

#include <filesystem>
#include <vector>

#include <cmath>

#include <glm/glm.hpp>

namespace meshes {

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
};

struct BoundingBox {
    glm::vec3 min, max;
};

class Mesh {
    std::vector<VertexData> vertices_;
    std::vector<uint16_t> indices_;

    Mesh(std::vector<VertexData>&& vertices, std::vector<uint16_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

public:
    static Mesh load(const std::filesystem::path& path) {

    }

    static Mesh rect();
    static Mesh cube();
    static Mesh sphere(uint32_t row, uint32_t column, float radius);
    static Mesh torus(uint32_t row, uint32_t column, float inner_radius, float outer_radius);
    static Mesh frame();

    auto& vertices() const noexcept { return vertices_; }
    auto& indices() const noexcept { return indices_; }

    BoundingBox bounding_box() const {
        auto min = glm::vec3(std::numeric_limits<float>::max());
        auto max = glm::vec3(std::numeric_limits<float>::lowest());

        for(const auto& v : vertices_) {
            min = glm::min(min, v.position);
            max = glm::max(max, v.position);
        }

        return { min, max };
    }
};

}