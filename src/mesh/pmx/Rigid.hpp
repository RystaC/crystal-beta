#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct Rigid {
    std::filesystem::path name;
    std::filesystem::path name_en;
    int32_t index;
    uint8_t group;
    uint16_t group_flag;
    uint8_t topology;
    glm::vec3 size;
    glm::vec3 position;
    glm::vec3 rotate_rad;
    float mass;
    float trans_atten;
    float rot_atten;
    float repulsion;
    float friction;
    uint8_t calc_type;
};

inline Rigid read_rigid(std::ifstream& ifs, uint8_t bone_index_size, bool is_utf8) {
    Rigid rigid{};

    // name
    rigid.name = std::move(read_text(ifs, is_utf8));
    rigid.name_en = std::move(read_text(ifs, is_utf8));
    // bone index
    ifs.read(reinterpret_cast<char*>(&rigid.index), bone_index_size);
    // group
    ifs.read(reinterpret_cast<char*>(&rigid.group), sizeof(uint8_t));
    // group flag
    ifs.read(reinterpret_cast<char*>(&rigid.group_flag), sizeof(uint16_t));
    // topology
    ifs.read(reinterpret_cast<char*>(&rigid.topology), sizeof(uint8_t));
    // size
    ifs.read(reinterpret_cast<char*>(&rigid.size), sizeof(glm::vec3));
    // position
    ifs.read(reinterpret_cast<char*>(&rigid.position), sizeof(glm::vec3));
    // rotation
    ifs.read(reinterpret_cast<char*>(&rigid.rotate_rad), sizeof(glm::vec3));
    // mass
    ifs.read(reinterpret_cast<char*>(&rigid.mass), sizeof(float));
    // translation attenuation
    ifs.read(reinterpret_cast<char*>(&rigid.trans_atten), sizeof(float));
    // rotation attenuation
    ifs.read(reinterpret_cast<char*>(&rigid.rot_atten), sizeof(float));
    // repulsion
    ifs.read(reinterpret_cast<char*>(&rigid.repulsion), sizeof(float));
    // friction
    ifs.read(reinterpret_cast<char*>(&rigid.friction), sizeof(float));
    // calculation type
    ifs.read(reinterpret_cast<char*>(&rigid.calc_type), sizeof(uint8_t));

    return rigid;
}

}

}