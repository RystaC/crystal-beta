#pragma once

#include "common.hpp"
#include "HalfEdge.hpp"

namespace mesh {

// TODO: use kd-tree or BVH to build meshlet
class Meshlet {
public:
    struct Data {
        glm::vec3 aabb_min;
        uint32_t index_offset;
        glm::vec3 aabb_max;
        uint32_t index_count;
    };

private:
    std::vector<VertexAttribute> vertices_;
    std::vector<uint32_t> indices_;
    std::vector<Data> meshlets_;

    Meshlet(std::vector<VertexAttribute>&& vertices, std::vector<uint32_t>&& indices, std::vector<Data>&& meshlets) noexcept :
        vertices_(vertices), indices_(indices), meshlets_(meshlets)
    {}

public:
    static Meshlet generate_meshlet_kdtree(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices);

    const auto& vertices() const noexcept { return vertices_; }
    auto& vertices() noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    auto& indices() noexcept { return indices_; }
    const auto& meshlets() const noexcept { return meshlets_; }
    auto& meshlets() noexcept { return meshlets_; }

    void print_statistics(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) const;
};

}