#pragma once

#include <format>
#include <iostream>
#include <map>
#include <vector>

#include <cstdint>

namespace mesh {

class HalfEdge {
    std::map<std::pair<uint32_t, uint32_t>, int32_t> opposites_;

    HalfEdge(std::map<std::pair<uint32_t, uint32_t>, int32_t>&& opposites) noexcept : opposites_(opposites) {}

public:
    static HalfEdge create_half_edge(const std::vector<uint32_t>& indices);

    const auto& opposites() const noexcept { return opposites_; }
    auto& opposites() noexcept { return opposites_; }
};

}