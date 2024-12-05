#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

// struct BDEF1 {
//     int32_t bone_index;
// };

// struct BDEF2 {
//     int32_t bone_indices[2];
//     float weight;
// };

// struct BDEF4 {
//     int32_t bone_indices[4];
//     float weights[4];
// };

// struct SDEF {
//     int32_t bone_indices[2];
//     float weight;
//     glm::vec3 c;
//     glm::vec3 r[2];
// };

// struct Vertex {
//     glm::vec3 position;
//     glm::vec3 normal;
//     glm::vec2 uv;
//     glm::vec4 additional_uv[4];
//     uint8_t weight_type;
//     union BoneWeight {
//         BDEF1 bdef1;
//         BDEF2 bdef2;
//         BDEF4 bdef4;
//         SDEF sdef;
//     } bone_weight;
//     float edge_mult;
// };

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::ivec4 bone_indices;
    glm::vec4 bone_weights;
    float edge_mult;
};

inline Vertex read_vertex(std::ifstream& ifs, uint8_t add_uv_count, uint8_t bone_index_size) {
    Vertex vertex{};
    // position
    ifs.read(reinterpret_cast<char*>(&vertex.position), sizeof(glm::vec3));
    // normal
    ifs.read(reinterpret_cast<char*>(&vertex.normal), sizeof(glm::vec3));
    // uv
    ifs.read(reinterpret_cast<char*>(&vertex.uv), sizeof(glm::vec2));
    // addtional uvs (ignore)
    for(uint8_t i = 0; i < add_uv_count; ++i) {
        ifs.seekg(sizeof(glm::vec4), std::ios::cur);
    }
    // weight type
    uint8_t weight_type{};
    ifs.read(reinterpret_cast<char*>(&weight_type), sizeof(uint8_t));
    // bone indices/weights
    vertex.bone_indices = glm::ivec4(-1);
    switch(weight_type) {
        // BDEF1
        case 0:
            vertex.bone_indices.x = read_index(ifs, bone_index_size);
            vertex.bone_weights.x = 1.0f;
            break;
        // BDEF2
        case 1:
            vertex.bone_indices.x = read_index(ifs, bone_index_size);
            vertex.bone_indices.y = read_index(ifs, bone_index_size);
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.x), sizeof(float));
            vertex.bone_weights.y = 1.0f - vertex.bone_weights.x;
            break;
        // BDEF4
        case 2:
            vertex.bone_indices.x = read_index(ifs, bone_index_size);
            vertex.bone_indices.y = read_index(ifs, bone_index_size);
            vertex.bone_indices.z = read_index(ifs, bone_index_size);
            vertex.bone_indices.w = read_index(ifs, bone_index_size);
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.x), sizeof(float));
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.y), sizeof(float));
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.z), sizeof(float));
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.w), sizeof(float));
            // maybe sum of weights is not 1 -> normalize
            vertex.bone_weights = glm::normalize(vertex.bone_weights);
            break;
        // SDEF
        case 3:
            vertex.bone_indices.x = read_index(ifs, bone_index_size);
            vertex.bone_indices.y = read_index(ifs, bone_index_size);
            ifs.read(reinterpret_cast<char*>(&vertex.bone_weights.x), sizeof(float));
            vertex.bone_weights.y = 1.0f - vertex.bone_weights.x;
            // ignore SDEF parameters
            ifs.seekg(sizeof(glm::vec3) * 3, std::ios::cur);
            break;
        default:
            break;
    }
    // edge mult
    ifs.read(reinterpret_cast<char*>(&vertex.edge_mult), sizeof(float));

    return vertex;
}

}

}