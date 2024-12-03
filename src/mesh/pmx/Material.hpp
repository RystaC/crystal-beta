#pragma once

#include "../common.hpp"
#include "utils.hpp"

namespace mesh {

namespace pmx {

struct Material {
    // 0
    glm::vec4 diffuse;
    // 16
    glm::vec3 specular;
    // 28
    float specular_intensity;
    // 32
    glm::vec3 ambient;
    // 44
    float edge_size;
    // 48
    glm::vec4 edge_color;
    // 64
    // texture / sphere / toon
    glm::ivec3 texture_indices;
    // 76
    uint32_t vertex_count;
    // 80
    uint8_t flags;
    // 84
    uint8_t sphere_mode;

    // meta data
    std::filesystem::path name;
    std::filesystem::path name_en;
    std::filesystem::path memo;
};

inline Material read_material(std::ifstream& ifs, uint8_t tex_index_size, bool is_utf8 = false) {
    Material material{};
    material.texture_indices = glm::ivec3(-1);
    // name
    material.name = std::move(read_text(ifs, is_utf8));
    material.name_en = std::move(read_text(ifs, is_utf8));
    // diffuse
    ifs.read(reinterpret_cast<char*>(&material.diffuse), sizeof(glm::vec4));
    // specular
    ifs.read(reinterpret_cast<char*>(&material.specular), sizeof(glm::vec3));
    // specular intensity
    ifs.read(reinterpret_cast<char*>(&material.specular_intensity), sizeof(float));
    // ambient
    ifs.read(reinterpret_cast<char*>(&material.ambient), sizeof(glm::vec3));
    // flags
    ifs.read(reinterpret_cast<char*>(&material.flags), sizeof(uint8_t));
    // edge color
    ifs.read(reinterpret_cast<char*>(&material.edge_color), sizeof(glm::vec4));
    // edge size
    ifs.read(reinterpret_cast<char*>(&material.edge_size), sizeof(float));
    // normal texture index
    material.texture_indices.x = read_index(ifs, tex_index_size);
    // sphere texture index
    material.texture_indices.y = read_index(ifs, tex_index_size);
    // sphere mode
    ifs.read(reinterpret_cast<char*>(&material.sphere_mode), sizeof(uint8_t));
    // toon flag
    uint8_t toon_flag{};
    ifs.read(reinterpret_cast<char*>(&toon_flag), sizeof(uint8_t));
    if(!toon_flag) {
        // toon texture index
        material.texture_indices.z = read_index(ifs, tex_index_size);
    }
    else {
        // shared toon index (ignore)
        ifs.seekg(sizeof(uint8_t), std::ios::cur);
        // ifs.read(reinterpret_cast<char*>(&material.toon_tex_index), sizeof(uint8_t));
    }
    // memo
    material.memo = std::move(read_text(ifs, is_utf8));
    // vertex count
    ifs.read(reinterpret_cast<char*>(&material.vertex_count), sizeof(uint32_t));

    return material;
}

}

}