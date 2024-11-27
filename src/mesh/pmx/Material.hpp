#pragma once

#include "../common.hpp"
#include "utils.hpp"

namespace mesh {

namespace pmx {

struct Material {
    std::filesystem::path name;
    std::filesystem::path name_en;
    glm::vec4 diffuse;
    glm::vec3 specular;
    float specular_intensity;
    glm::vec3 ambient;
    uint8_t flags;
    glm::vec4 edge_color;
    float edge_size;
    int32_t tex_index;
    int32_t sphere_tex_index;
    uint8_t sphere_mode;
    uint8_t toon_flag;
    int32_t toon_tex_index;
    std::filesystem::path memo;
    uint32_t vertex_count;
};

inline Material read_material(std::ifstream& ifs, uint8_t tex_index_size, bool is_utf8 = false) {
    Material material{};
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
    ifs.read(reinterpret_cast<char*>(&material.tex_index), tex_index_size);
    // sphere texture index
    ifs.read(reinterpret_cast<char*>(&material.sphere_tex_index), tex_index_size);
    // sphere mode
    ifs.read(reinterpret_cast<char*>(&material.sphere_mode), sizeof(uint8_t));
    // toon flag
    ifs.read(reinterpret_cast<char*>(&material.toon_flag), sizeof(uint8_t));
    if(!material.toon_flag) {
        // toon texture index
        ifs.read(reinterpret_cast<char*>(&material.toon_tex_index), tex_index_size);
    }
    else {
        // shared toon index
        ifs.read(reinterpret_cast<char*>(&material.toon_tex_index), sizeof(uint8_t));
    }
    // memo
    material.memo = std::move(read_text(ifs, is_utf8));
    // vertex count
    ifs.read(reinterpret_cast<char*>(&material.vertex_count), sizeof(uint32_t));

    return material;
}

}

}