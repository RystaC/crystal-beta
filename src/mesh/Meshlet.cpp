#include "Meshlet.hpp"

namespace mesh {

Meshlet Meshlet::generate_meshlet_kdtree(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) {
    constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
    constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();

    std::vector<VertexAttribute> vertices_copied = vertices;

    std::vector<uint32_t> meshlet_indices_counts{};
    std::vector<Data> meshlets{};
    std::vector<uint32_t> new_indices{};
    new_indices.reserve(indices.size());

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
            Data meshlet{};
            meshlet.index_offset = static_cast<uint32_t>(new_indices.size());
            meshlet.index_count = static_cast<uint32_t>(dist * 3);
            
            auto box = AABB{glm::vec3(FLOAT_MAX), glm::vec3(FLOAT_MIN)};
            std::for_each(b, e, [&](const auto& i) {
                auto i0 = indices[i*3+0]; auto i1 = indices[i*3+1]; auto i2 = indices[i*3+2];
                // rearange indices
                new_indices.emplace_back(i0); new_indices.emplace_back(i1); new_indices.emplace_back(i2);
                // generate bounding box
                auto v0 = vertices[i0].position; auto v1 = vertices[i1].position; auto v2 = vertices[i2].position;
                box.min = glm::min(box.min, v0); box.min = glm::min(box.min, v1); box.min = glm::min(box.min, v2);
                box.max = glm::max(box.max, v0); box.max = glm::max(box.max, v1); box.max = glm::max(box.max, v2);
            });

            meshlet.aabb_min = box.min;
            meshlet.aabb_max = box.max;
            
            meshlets.emplace_back(std::move(meshlet));
            continue;
        }

        // split space by specific axis
        std::nth_element(b, b + dist/2, e, [&](const auto& a, const auto& b) { return centroids[a][axis] < centroids[b][axis]; });

        queue.push({b, b + dist/2, (axis + 1) % 3});
        queue.push({b + dist/2, e, (axis + 1) % 3});
    }

    return {std::move(vertices_copied), std::move(new_indices), std::move(meshlets)};
}

void Meshlet::print_statistics(const std::vector<VertexAttribute>& vertices, const std::vector<uint32_t>& indices) const {
    std::cerr << std::format("# of meshlets = {}", meshlets_.size()) << std::endl;

    // calculate total bounding box area for previous mesh
    float mesh_area = 0.0f;
    constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
    constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
    for(size_t i = 0; i < meshlets_.size(); ++i) {
        auto box = AABB{glm::vec3(FLOAT_MAX), glm::vec3(FLOAT_MIN)};
        for(size_t offset = 0; offset < meshlets_[i].index_count; ++offset) {
            auto vertex = vertices[indices[meshlets_[i].index_offset + offset]].position;
            box.min = glm::min(box.min, vertex);
            box.max = glm::max(box.max, vertex);
        }
        mesh_area += box.area();
    }

    // calculate total bounding box area for meshlet
    float meshlet_area = 0.0f;
    for(const auto& b : meshlets_) {
        meshlet_area += aabb_area(b.aabb_min, b.aabb_max);
    }
    std::cerr << std::format("total bounding box area (mesh) = {:.2f}", mesh_area) << std::endl;
    std::cerr << std::format("total bounding box area (meshlet) = {:.2f}", meshlet_area) << std::endl;
}

}