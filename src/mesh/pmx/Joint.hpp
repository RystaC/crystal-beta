#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct Joint {
    std::filesystem::path name;
    std::filesystem::path name_en;
    uint8_t type;
    int32_t index_a;
    int32_t index_b;
    glm::vec3 position;
    glm::vec3 rotate_rad;
    glm::vec3 trans_lower;
    glm::vec3 trans_upper;
    glm::vec3 rot_lower_rad;
    glm::vec3 rot_upper_rad;
    glm::vec3 k_trans;
    glm::vec3 k_rot;
};

inline Joint read_joint(std::ifstream& ifs, uint8_t rigid_index_size, bool is_utf8) {
    Joint joint{};

    // name
    joint.name = std::move(read_text(ifs, is_utf8));
    joint.name_en = std::move(read_text(ifs, is_utf8));
    // type (must be 0 in ver2.0)
    ifs.read(reinterpret_cast<char*>(&joint.type), sizeof(uint8_t));
    // rigid A
    ifs.read(reinterpret_cast<char*>(&joint.index_a), rigid_index_size);
    // rigid B
    ifs.read(reinterpret_cast<char*>(&joint.index_b), rigid_index_size);
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