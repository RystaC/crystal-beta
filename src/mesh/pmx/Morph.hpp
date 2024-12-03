#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct VertexMorph {
    glm::vec3 offset;
    int32_t index;
};

inline VertexMorph read_vertex_morph(std::ifstream& ifs, uint8_t vertex_index_size) {
    VertexMorph morph{};

    // index
    morph.index = read_index(ifs, vertex_index_size);
    // position offset
    ifs.read(reinterpret_cast<char*>(&morph.offset), sizeof(glm::vec3));

    return morph;
}

struct UVMorph {
    glm::vec4 offset;
    int32_t index;
};

inline UVMorph read_uv_morph(std::ifstream& ifs, uint8_t vertex_index_size) {
    UVMorph morph{};
    
    // index
    morph.index = read_index(ifs, vertex_index_size);
    // uv offset
    ifs.read(reinterpret_cast<char*>(&morph.offset), sizeof(glm::vec4));

    return morph;
}

struct BoneMorph {
    glm::vec3 translate;
    int32_t index;
    glm::vec4 rotate_quat;
};

inline BoneMorph read_bone_morph(std::ifstream& ifs, uint8_t bone_index_size) {
    BoneMorph morph{};
    
    // index
    morph.index = read_index(ifs, bone_index_size);
    // translate offset
    ifs.read(reinterpret_cast<char*>(&morph.translate), sizeof(glm::vec3));
    // rotate offset
    ifs.read(reinterpret_cast<char*>(&morph.rotate_quat), sizeof(glm::vec4));

    return morph;
}

struct MaterialMorph {
    glm::vec4 diffuse;
    glm::vec3 specular;
    float specular_intensity;
    glm::vec3 ambient;
    float edge_size;
    glm::vec4 edge_color;
    glm::vec4 tex_coef;
    glm::vec4 sphere_tex_coef;
    glm::vec4 toon_tex_coef;
    int32_t index;
    uint8_t calc_mode;
};

inline MaterialMorph read_material_morph(std::ifstream& ifs, uint8_t material_index_size) {
    MaterialMorph morph{};

    // index
    morph.index = read_index(ifs, material_index_size);
    // calculation mode
    ifs.read(reinterpret_cast<char*>(&morph.calc_mode), sizeof(uint8_t));
    // diffuse
    ifs.read(reinterpret_cast<char*>(&morph.diffuse), sizeof(glm::vec4));
    // specular
    ifs.read(reinterpret_cast<char*>(&morph.specular), sizeof(glm::vec3));
    // specular intensity
    ifs.read(reinterpret_cast<char*>(&morph.specular_intensity), sizeof(float));
    // ambient
    ifs.read(reinterpret_cast<char*>(&morph.ambient), sizeof(glm::vec3));
    // edge color
    ifs.read(reinterpret_cast<char*>(&morph.edge_color), sizeof(glm::vec4));
    // edge size
    ifs.read(reinterpret_cast<char*>(&morph.edge_size), sizeof(float));
    // texture coefficient
    ifs.read(reinterpret_cast<char*>(&morph.tex_coef), sizeof(glm::vec4));
    // sphere texture coefficient
    ifs.read(reinterpret_cast<char*>(&morph.sphere_tex_coef), sizeof(glm::vec4));
    // toon texture coefficient
    ifs.read(reinterpret_cast<char*>(&morph.toon_tex_coef), sizeof(glm::vec4));

    return morph;
}

struct GroupMorph {
    int32_t index;
    float rate;
};

inline GroupMorph read_group_morph(std::ifstream& ifs, uint8_t morph_index_size) {
    GroupMorph morph{};

    // index
    morph.index = read_index(ifs, morph_index_size);
    // rate
    ifs.read(reinterpret_cast<char*>(&morph.rate), sizeof(float));

    return morph;
}

struct Morph {
    std::filesystem::path name;
    std::filesystem::path name_en;
    uint8_t panel;
    uint8_t type;
    uint32_t offset_count;
    union Offset {
        VertexMorph vertex;
        UVMorph uv;
        BoneMorph bone;
        MaterialMorph material;
        GroupMorph group;
    };
    std::vector<Offset> offsets;
};

inline Morph read_morph(std::ifstream& ifs,
    uint8_t vertex_index_size, uint8_t bone_index_size,
    uint8_t material_index_size, uint8_t morph_index_size,
    bool is_utf8)
{
    Morph morph{};

    // name
    morph.name = std::move(read_text(ifs, is_utf8));
    morph.name_en = std::move(read_text(ifs, is_utf8));
    // panel
    ifs.read(reinterpret_cast<char*>(&morph.panel), sizeof(uint8_t));
    // type
    ifs.read(reinterpret_cast<char*>(&morph.type), sizeof(uint8_t));
    // number of morphs
    ifs.read(reinterpret_cast<char*>(&morph.offset_count), sizeof(uint32_t));
    morph.offsets.resize(morph.offset_count);

    switch(morph.type) {
        // group
        case 0:
            for(size_t i = 0; i < morph.offsets.size(); ++i) {
                morph.offsets[i].group = std::move(read_group_morph(ifs, morph_index_size));
            }
            break;
        // vertex
        case 1:
            for(size_t i = 0; i < morph.offsets.size(); ++i) {
                morph.offsets[i].vertex = std::move(read_vertex_morph(ifs, vertex_index_size));
            }
            break;
        // bone
        case 2:
            for(size_t i = 0; i < morph.offsets.size(); ++i) {
                morph.offsets[i].bone = std::move(read_bone_morph(ifs, bone_index_size));
            }
            break;
        // uv
        case 3:
        // additonal uv1
        case 4:
        // additonal uv2
        case 5:
        // additonal uv3
        case 6:
        // additonal uv4
        case 7:
            for(size_t i = 0; i < morph.offsets.size(); ++i) {
                morph.offsets[i].uv = std::move(read_uv_morph(ifs, vertex_index_size));
            }
            break;
        // material
        case 8:
            for(size_t i = 0; i < morph.offsets.size(); ++i) {
                morph.offsets[i].material = std::move(read_material_morph(ifs, material_index_size));
            }
            break;
        default:
            break;
    }

    return morph;
}

}

}