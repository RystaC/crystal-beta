#include "HalfEdge.hpp"

namespace mesh {

HalfEdge HalfEdge::create_half_edge(const std::vector<uint32_t>& indices) {
    auto index_count = indices.size();
    auto vertex_count = index_count / 3;

    std::map<std::pair<uint32_t, uint32_t>, int32_t> opposites{};

    constexpr std::pair<size_t, size_t> edge_indices[3] = {
        { 0, 1 },
        { 1, 2 },
        { 2, 0 },
    };

    for(size_t i = 0; i < index_count; i += 3) {
        for(size_t j = 0; j < 3; ++j) {
            // half edge
            auto half_edge = std::make_pair(indices[i+edge_indices[j].first], indices[i+edge_indices[j].second]);
            // opposite edge
            auto opposite_edge = std::make_pair(half_edge.second, half_edge.first);

            if(opposites.contains(opposite_edge)) {
                opposites[half_edge] = -static_cast<int32_t>(opposites[opposite_edge] + 1);
                opposites[opposite_edge] = static_cast<int32_t>(i + j);
            }
            else {
                opposites[half_edge] = -static_cast<int32_t>(i + j + 1);
            }
        }
    }

    return { std::move(opposites) };
}

void HalfEdge::print_opposites(const std::vector<uint32_t>& indices) const {
    for(size_t i = 0; i < indices.size(); i += 3) {
        auto e0 = std::make_pair(indices[i+0], indices[i+1]);
        auto e1 = std::make_pair(indices[i+1], indices[i+2]);
        auto e2 = std::make_pair(indices[i+2], indices[i+0]);

        std::cerr << std::format("face #{}: edge 0 = {}, edge 1 = {}, edge 2 = {}", i/3, opposites_.at(e0)/3, opposites_.at(e1)/3, opposites_.at(e2)/3) << std::endl;
    }
}

}