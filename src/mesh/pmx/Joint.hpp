#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct Joint {
    // 0
    glm::vec3 position;
    // 12
    int32_t index_a;
    // 16
    glm::vec3 rotate_rad;
    // 28
    int32_t index_b;
    // 32
    glm::vec3 trans_lower;
    // 48
    alignas(16) glm::vec3 trans_upper;
    // 64
    alignas(16) glm::vec3 rot_lower_rad;
    // 80
    alignas(16) glm::vec3 rot_upper_rad;
    // 96
    alignas(16) glm::vec3 k_trans;
    // 112
    alignas(16) glm::vec3 k_rot;
    // 128
    uint8_t type;

    // meta data
    std::filesystem::path name;
    std::filesystem::path name_en;
};

inline Joint read_joint(std::ifstream& ifs, uint8_t rigid_index_size, bool is_utf8) {
    Joint joint{};

    // name
    joint.name = std::move(read_text(ifs, is_utf8));
    joint.name_en = std::move(read_text(ifs, is_utf8));
    // type (must be 0 in ver2.0)
    ifs.read(reinterpret_cast<char*>(&joint.type), sizeof(uint8_t));
    // rigid A
    joint.index_a = read_index(ifs, rigid_index_size);
    // rigid B
    joint.index_b = read_index(ifs, rigid_index_size);
    // position
    ifs.read(reinterpret_cast<char*>(&joint.position), sizeof(glm::vec3));
    // rotation
    ifs.read(reinterpret_cast<char*>(&joint.rotate_rad), sizeof(glm::vec3));
    // translation lower bound
    ifs.read(reinterpret_cast<char*>(&joint.trans_lower), sizeof(glm::vec3));
    // translation upper bound
    ifs.read(reinterpret_cast<char*>(&joint.trans_upper), sizeof(glm::vec3));
    // rotation lower bound
    ifs.read(reinterpret_cast<char*>(&joint.rot_lower_rad), sizeof(glm::vec3));
    // rotation upper bound
    ifs.read(reinterpret_cast<char*>(&joint.rot_upper_rad), sizeof(glm::vec3));
    // spring constant translation
    ifs.read(reinterpret_cast<char*>(&joint.k_trans), sizeof(glm::vec3));
    // spring constant rotation
    ifs.read(reinterpret_cast<char*>(&joint.k_rot), sizeof(glm::vec3));

    return joint;
}

}

}