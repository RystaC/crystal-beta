#include "Meshlet.hpp"

namespace mesh {

Meshlet Meshlet::generate_meshlet(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices, const HalfEdge& half_edge, uint32_t faces_per_meshlet) {
    std::vector<VertexAttribute> vertices_copied = vertices;
    std::vector<uint32_t> indices_copied = indices;
    std::vector<uint32_t> new_indices(indices.size());

    constexpr uint32_t USED_FACE = std::numeric_limits<uint32_t>::max();

    for(size_t src_idx = 0, dst_idx = 0; src_idx < indices.size();) {
        // if face is already used for meshlet -> skip
        if(indices_copied[src_idx] == USED_FACE) {
            src_idx += 3;
            continue;
        }

        std::cerr << std::format("src idx = ({}, {}, {}), dst idx = ({}, {}, {})", src_idx, src_idx+1, src_idx+2, dst_idx, dst_idx+1, dst_idx+2) << std::endl;

        // pick a face as meshlet seed
        auto v0 = indices_copied[src_idx+0]; indices_copied[src_idx+0] = USED_FACE;
        auto v1 = indices_copied[src_idx+1]; indices_copied[src_idx+1] = USED_FACE;
        auto v2 = indices_copied[src_idx+2]; indices_copied[src_idx+2] = USED_FACE;
        new_indices[dst_idx+0] = v0;
        new_indices[dst_idx+1] = v1;
        new_indices[dst_idx+2] = v2;
        src_idx += 3; dst_idx += 3;

        // push edges
        std::queue<std::pair<uint32_t, uint32_t>> queue{};
        queue.push({v0, v1});
        queue.push({v1, v2});
        queue.push({v2, v0});
        uint32_t face_count = 0;
        while(face_count < faces_per_meshlet && !queue.empty()) {
            auto edge = queue.front(); queue.pop();

            // if edge if boundary -> go next
            std::cerr << std::format("check opposite for edge ({}, {})", edge.first, edge.second) << std::endl;
            auto opposite_idx = half_edge.opposites().at(edge);
            // std::cerr << "opposite: " << opposite_idx << std::endl;
            if(opposite_idx < 0) {
                std::cerr << "*** skipped by boundary ***" << std::endl;
                continue;
            }

            // if connected face is already used -> go next
            auto connected_face_idx = opposite_idx / 3;
            // std::cerr << "face: " << connected_face_idx << std::endl;
            if(indices_copied[connected_face_idx] == USED_FACE) {
                std::cerr << "*** skipped by connected face used ***" << std::endl;
                continue;
            }

            // if face of edge is already used -> go next
            std::cerr << "check itself" << std::endl;
            auto face_idx = half_edge.opposites().at({edge.second, edge.first}) / 3;
            if(indices_copied[face_idx] == USED_FACE) {
                std::cerr << "*** skipped by face used ***" << std::endl;
                continue;
            }

            // extract connected face
            std::cerr << std::format("connected face idx = ({}, {}, {}), dst idx = ({}, {}, {})", connected_face_idx, connected_face_idx+1, connected_face_idx+2, dst_idx, dst_idx+1, dst_idx+2) << std::endl; 
            auto new_v0 = indices_copied[connected_face_idx+0]; indices_copied[connected_face_idx+0] = USED_FACE;
            auto new_v1 = indices_copied[connected_face_idx+1]; indices_copied[connected_face_idx+1] = USED_FACE;
            auto new_v2 = indices_copied[connected_face_idx+2]; indices_copied[connected_face_idx+2] = USED_FACE;
            new_indices[dst_idx+0] = new_v0;
            new_indices[dst_idx+1] = new_v1;
            new_indices[dst_idx+2] = new_v2;
            face_count += 1; dst_idx += 3;

            // add new edges
            // TODO: we can remove used edge (extract index like connected_face % 3)
            queue.push({new_v0, new_v1});
            queue.push({new_v1, new_v2});
            queue.push({new_v2, new_v0});
        }
    }

    return { std::move(vertices_copied), std::move(new_indices) };
}

void Meshlet::print_indices() const {
    for(size_t i = 0; i < indices_.size(); i += 3) {
        std::cerr << std::format("face #{}: ({}, {}, {})", i/3, indices_[i+0], indices_[i+1], indices_[i+2]) << std::endl;
    }
}

}