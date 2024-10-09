#pragma once

#include "common.hpp"
#include "HalfEdge.hpp"

namespace mesh {

// TODO: use bounding volume hierarchy to build meshlet
class Meshlet {
    std::vector<VertexAttribute> vertices_;
    std::vector<uint32_t> indices_;

    Meshlet(std::vector<VertexAttribute>&& vertices, std::vector<uint32_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

public:
    static Meshlet generate_meshlet(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices, const HalfEdge& half_edge, uint32_t faces_per_meshlet = 32);

    const auto& vertices() const noexcept { return vertices_; }
    auto& vertices() noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    auto& indices() noexcept { return indices_; }

    void print_indices() const;
};

}