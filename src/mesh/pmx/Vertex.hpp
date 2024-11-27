#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct BDEF1 {
    int32_t bone_index;
};

inline BDEF1 read_bdef1(std::istream& ifs, uint8_t index_size) {
    BDEF1 weight{};
    ifs.read(reinterpret_cast<char*>(&weight.bone_index), index_size);

    return weight;
}

struct BDEF2 {
    int32_t bone_indices[2];
    float weight;
};

inline BDEF2 read_bdef2(std::istream& ifs, uint8_t index_size) {
    BDEF2 weight{};
    ifs.read(reinterpret_cast<char*>(weight.bone_indices), index_size * 2);
    ifs.read(reinterpret_cast<char*>(&weight.weight), sizeof(float));

    return weight;
}

struct BDEF4 {
    int32_t bone_indices[4];
    float weights[4];
};

inline BDEF4 read_bdef4(std::istream& ifs, uint8_t index_size) {
    BDEF4 weight{};
    ifs.read(reinterpret_cast<char*>(weight.bone_indices), index_size * 4);
    ifs.read(reinterpret_cast<char*>(weight.weights), sizeof(float) * 4);

    return weight;
}

struct SDEF {
    int32_t bone_indices[2];
    float weight;
    glm::vec3 c;
    glm::vec3 r[2];
};

inline SDEF read_sdef(std::istream& ifs, uint8_t index_size) {
    SDEF weight{};
    ifs.read(reinterpret_cast<char*>(weight.bone_indices), index_size * 2);
    ifs.read(reinterpret_cast<char*>(&weight.weight), sizeof(float));
    ifs.read(reinterpret_cast<char*>(&weight.c), sizeof(glm::vec3));
    ifs.read(reinterpret_cast<char*>(weight.r), sizeof(glm::vec3) * 2);

    return weight;
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 additional_uv[4];
    uint8_t weight_type;
    union BoneWeight {
        BDEF1 bdef1;
        BDEF2 bdef2;
        BDEF4 bdef4;
        SDEF sdef;
    } bone_weight;
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
    // addtional uvs
    for(uint8_t i = 0; i < add_uv_count; ++i) {
        ifs.read(reinterpret_cast<char*>(&vertex.additional_uv[i]), sizeof(glm::vec4));
    }
    // weight type
    ifs.read(reinterpret_cast<char*>(&vertex.weight_type), sizeof(uint8_t));
    // bone weight
    switch(vertex.weight_type) {
        // BDEF1
        case 0:
            vertex.bone_weight.bdef1 = read_bdef1(ifs, bone_index_size);
            break;
        // BDEF2
        case 1:
            vertex.bone_weight.bdef2 = read_bdef2(ifs, bone_index_size);
            break;
        // BDEF4
        case 2:
            vertex.bone_weight.bdef4 = read_bdef4(ifs, bone_index_size);
            break;
        // SDEF
        case 3:
            vertex.bone_weight.sdef = read_sdef(ifs, bone_index_size);
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