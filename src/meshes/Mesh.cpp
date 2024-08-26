#include "Mesh.hpp"

namespace meshes {

Mesh Mesh::rect() {
    std::vector<VertexData> vertices = {
        // z-
        { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
    };
    std::vector<uint16_t> indices = {
        0, 1, 2,
        1, 3, 2,
    };

    return { std::move(vertices), std::move(indices) };
}

Mesh Mesh::cube() {
    std::vector<VertexData> vertices = {
        // x+
        { glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        // x-
        { glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(1.0f) },
        // y+
        { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(1.0f) },
        // y-
        { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(1.0f) },
        // z+
        { glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(1.0f) },
        // z-
        { glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec4(1.0f) },
    };
    std::vector<uint16_t> indices = {
        // x+
        0+0, 0+1, 0+2,
        0+1, 0+3, 0+2,
        // x-
        4+0, 4+1, 4+2,
        4+1, 4+3, 4+2,
        // y+
        8+0, 8+1, 8+2,
        8+1, 8+3, 8+2,
        // y-
        16+0, 16+1, 16+2,
        16+1, 16+3, 16+2,
        // z+
        20+0, 20+1, 20+2,
        20+1, 20+3, 20+2,
        // z-
        24+0, 24+1, 24+2,
        24+1, 24+3, 24+2,    
    };

    return { std::move(vertices), std::move(indices) };
}

Mesh Mesh::sphere(uint32_t row, uint32_t column, float radius) {
    constexpr float PI = 3.14159265359f;

    std::vector<VertexData> vertices{};
    std::vector<uint16_t> indices{};
    vertices.reserve((row+1)*(column*1));
    indices.reserve(row*column*6);

    for(auto i = 0u; i <= row; ++i) {
        float r = PI / row * i;
        float ry = std::cos(r);
        float rr = std::sin(r);
        for(auto ii = 0u; ii <= column; ++ii) {
            float tr = PI * 2.0f / column * ii;
            float tx = rr * radius * std::cos(tr);
            float ty = ry * radius;
            float tz = rr * radius * std::sin(tr);
            float rx = rr * std::cos(tr);
            float rz = rr * std::sin(tr);
            vertices.emplace_back(VertexData{ glm::vec3(tx, ty, tz), glm::vec3(rx, ry, rz), glm::vec4(1.0) });
        }
    }

    for(auto i = 0u; i < row; ++i) {
        for(auto ii = 0u; ii < column; ++ii) {
            uint16_t r = (column + 1) * i + ii;
            indices.push_back(r);
            indices.push_back(r+1);
            indices.push_back(r+column+2);
            indices.push_back(r);
            indices.push_back(r+column+2);
            indices.push_back(r+column+1);
        }
    }

    return { std::move(vertices), std::move(indices) };
}

Mesh Mesh::torus(uint32_t row, uint32_t column, float inner_radius, float outer_radius) {
    constexpr float PI = 3.14159265359f;

    std::vector<VertexData> vertices{};
    std::vector<uint16_t> indices{};
    vertices.reserve((row+1)*(column+1));
    indices.reserve(row*column*6);

    for(auto i = 0u; i <= row; ++i) {
        float r = PI * 2.0f / row * i;
        float rr = std::cos(r);
        float ry = std::sin(r);
        for(auto ii = 0u; ii <= column; ++ii) {
            float tr = PI * 2.0f / column * ii;
            float tx = (rr * inner_radius + outer_radius) * std::cos(tr);
            float ty = ry * inner_radius;
            float tz = (rr * inner_radius + outer_radius) * std::sin(tr);
            float rx = rr * std::cos(tr);
            float rz = rr * std::sin(tr);
            vertices.emplace_back(VertexData{ glm::vec3(tx, ty, tz), glm::vec3(rx, ry, rz), glm::vec4(1.0f) });
        }
    }

    for(auto i = 0u; i < row; ++i) {
        for(auto ii = 0u; ii < column; ++ii) {
            uint16_t r = (column + 1) * i + ii;
            indices.push_back(r);
            indices.push_back(r + column + 1);
            indices.push_back(r + 1);
            indices.push_back(r + column + 1);
            indices.push_back(r + column + 2);
            indices.push_back(r + 1);
        }
    }

    return { std::move(vertices), std::move(indices) };
}

Mesh Mesh::frame() {
    std::vector<VertexData> vertices = {
        { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
        { glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f), glm::vec4(1.0f) },
    };

    std::vector<uint16_t> indices = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7,
    };

    return { std::move(vertices), std::move(indices) };
}

}