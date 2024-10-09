#pragma once

#include "common.hpp"
#include "HalfEdge.hpp"

namespace mesh {

// TODO: use kd-tree or BVH to build meshlet
class Meshlet {
    std::vector<VertexAttribute> vertices_;
    std::vector<uint32_t> indices_;
    std::vector<uint32_t> meshlet_indices_counts_;
    std::vector<AABB> bounding_boxes_;

    Meshlet(std::vector<VertexAttribute>&& vertices, std::vector<uint32_t>&& indices, std::vector<uint32_t>&& meshlet_indices_counts, std::vector<AABB>&& bounding_boxes) noexcept :
        vertices_(vertices), indices_(indices), meshlet_indices_counts_(meshlet_indices_counts), bounding_boxes_(bounding_boxes)
    {}

public:
    static Meshlet generate_meshlet_kdtree(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices);

    const auto& vertices() const noexcept { return vertices_; }
    auto& vertices() noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    auto& indices() noexcept { return indices_; }
    const auto& meshlet_indices_counts() const noexcept { return meshlet_indices_counts_; }
    auto& meshlet_indices_counts() noexcept { return meshlet_indices_counts_; }
    const auto& bounding_boxes() const noexcept { return bounding_boxes_; }
    auto& bounding_boxes() noexcept { return bounding_boxes_; }

    void print_statistics(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) const;
};

}