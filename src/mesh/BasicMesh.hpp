#pragma once

#include "common.hpp"

namespace mesh {

class BasicMesh {
    std::vector<VertexAttribute> vertices_;
    std::vector<uint16_t> indices_;

    BasicMesh(std::vector<VertexAttribute>&& vertices, std::vector<uint16_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

public:
    static BasicMesh rect();
    static BasicMesh cube();
    static BasicMesh sphere(uint32_t row, uint32_t column, float radius);
    static BasicMesh torus(uint32_t row, uint32_t column, float inner_radius, float outer_radius);
    static BasicMesh frame();

    auto& vertices() const noexcept { return vertices_; }
    auto& indices() const noexcept { return indices_; }

    AABB bounding_box() const {
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