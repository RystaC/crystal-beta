#include "Meshlet.hpp"

namespace mesh {

Meshlet Meshlet::generate_meshlet_kdtree(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) {
    std::vector<VertexAttribute> vertices_copied = vertices;

    std::vector<uint32_t> meshlet_indices_counts{};

    std::vector<glm::vec3> centroids(indices.size() / 3);
    for(size_t i = 0; i < centroids.size(); ++i) {
        centroids[i] = (vertices[indices[i*3+0]].position + vertices[indices[i*3+1]].position + vertices[indices[i*3+2]].position) / 3.0f;
    }

    std::vector<uint32_t> face_indices(indices.size() / 3);
    std::iota(face_indices.begin(), face_indices.end(), 0);

    using iter_t = std::vector<uint32_t>::iterator;
    // ([iterator begin], [iterator end], [current split axis(0=x, 1=y, 2=z)])
    std::queue<std::tuple<iter_t, iter_t, uint32_t>> queue{};
    queue.push({face_indices.begin(), face_indices.end(), 0});

    while(!queue.empty()) {
        auto [b, e, axis] = queue.front(); queue.pop();
        auto dist = std::distance(b, e);
        // # of faces < bound -> end split
        if(dist <= 100) {
            meshlet_indices_counts.emplace_back(static_cast<uint32_t>(dist) * 3);
            continue;
        }

        // split space by specific axis
        std::nth_element(b, b + dist/2, e, [&](const auto& a, const auto& b) { return centroids[a][axis] < centroids[b][axis]; });

        queue.push({b, b + dist/2, (axis + 1) % 3});
        queue.push({b + dist/2, e, (axis + 1) % 3});
    }

    // rearange indices
    std::vector<uint32_t> new_indices(indices.size());
    for(size_t i = 0; i < face_indices.size(); ++i) {
        new_indices[i*3+0] = indices[face_indices[i]*3+0];
        new_indices[i*3+1] = indices[face_indices[i]*3+1];
        new_indices[i*3+2] = indices[face_indices[i]*3+2];
    }

    // generate bounding box for each meshlet
    std::vector<AABB> bounding_boxes(meshlet_indices_counts.size());
    constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
    constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
    for(size_t i = 0, base = 0; i < meshlet_indices_counts.size(); ++i) {
        auto box = AABB{glm::vec3(FLOAT_MAX), glm::vec3(FLOAT_MIN)};
        for(size_t offset = 0; offset < meshlet_indices_counts[i]; ++offset) {
            auto vertex = vertices[new_indices[base + offset]].position;
            box.min = glm::min(box.min, vertex);
            box.max = glm::max(box.max, vertex);
        }

        bounding_boxes.emplace_back(std::move(box));
    }

    return {std::move(vertices_copied), std::move(new_indices), std::move(meshlet_indices_counts), std::move(bounding_boxes)};
}

void Meshlet::print_statistics(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) const {
    std::cerr << std::format("# of meshlets = {}", meshlet_indices_counts_.size()) << std::endl;

    auto ave_meshlet_count = static_cast<float>(std::accumulate(meshlet_indices_counts_.begin(), meshlet_indices_counts_.end(), 0) / 3) / meshlet_indices_counts_.size();
    std::cerr << std::format("average meshlet count = {:.2f}" , ave_meshlet_count) << std::endl;

    // calculate total bounding box area for previous mesh
    float mesh_area = 0.0f;
    constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
    constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
    for(size_t i = 0, base = 0; i < meshlet_indices_counts_.size(); ++i) {
        auto box = AABB{glm::vec3(FLOAT_MAX), glm::vec3(FLOAT_MIN)};
        for(size_t offset = 0; offset < meshlet_indices_counts_[i]; ++offset) {
            auto vertex = vertices[indices[base + offset]].position;
            box.min = glm::min(box.min, vertex);
            box.max = glm::max(box.max, vertex);
        }
        mesh_area += box.area();
    }

    // calculate total bounding box area for meshlet
    float meshlet_area = 0.0f;
    for(const auto& b : bounding_boxes_) {
        meshlet_area += b.area();
    }
    std::cerr << std::format("total bounding box area (mesh) = {:.2f}", mesh_area) << std::endl;
    std::cerr << std::format("total bounding box area (meshlet) = {:.2f}", meshlet_area) << std::endl;
}

}