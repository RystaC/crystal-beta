#pragma once

#include "../common.hpp"

namespace mesh {

namespace pmx {

struct IKLink {
    int32_t index;
    bool is_limited;
    glm::vec3 lower_bound;
    glm::vec3 upper_bound;
};

inline IKLink read_ik_link(std::ifstream& ifs, uint8_t bone_index_size) {
    IKLink link{};

    // bone index
    ifs.read(reinterpret_cast<char*>(&link.index), bone_index_size);
    // angle constraint
    ifs.read(reinterpret_cast<char*>(&link.is_limited), sizeof(uint8_t));
    if(link.is_limited) {
        // lower bound
        ifs.read(reinterpret_cast<char*>(&link.lower_bound), sizeof(glm::vec3));
        ifs.read(reinterpret_cast<char*>(&link.upper_bound), sizeof(glm::vec3));
    }

    return link;
}

struct Bone {
    std::filesystem::path name;
    std::filesystem::path name_en;
    glm::vec3 position;
    int32_t parent_index;
    uint32_t hierarchy;
    uint16_t flags;
    union Connect {
        glm::vec3 offset;
        uint32_t dst_index;
    } connect;
    uint32_t giving_index;
    float giving_rate;
    glm::vec3 axis_dir;
    glm::vec3 x_dir;
    glm::vec3 z_dir;
    uint32_t key;
    uint32_t ik_target_index;
    uint32_t ik_iter_count;
    float ik_constraint;
    uint32_t ik_link_count;
    std::vector<IKLink> ik_link;
};

inline Bone read_bone(std::ifstream& ifs, uint8_t bone_index_size, bool is_utf8) {
    Bone bone{};

    // name
    bone.name = std::move(read_text(ifs, is_utf8));
    bone.name_en = std::move(read_text(ifs, is_utf8));

    // position
    ifs.read(reinterpret_cast<char*>(&bone.position), sizeof(glm::vec3));
    // parent index
    ifs.read(reinterpret_cast<char*>(&bone.parent_index), bone_index_size);
    // hierarchy
    ifs.read(reinterpret_cast<char*>(&bone.hierarchy), sizeof(uint32_t));
    // flags
    ifs.read(reinterpret_cast<char*>(&bone.flags), sizeof(uint16_t));
    // connectivity = 0 -> offset
    if(!(bone.flags & 0x0001)) {
        ifs.read(reinterpret_cast<char*>(&bone.connect.offset), sizeof(glm::vec3));
    }
    // connectivity = 1 -> bone index
    else {
        ifs.read(reinterpret_cast<char*>(&bone.connect.dst_index), bone_index_size);
    }
    // rotate | translate giving -> index and giving rate
    if((bone.flags & 0x0100) || (bone.flags & 0x0200)) {
        ifs.read(reinterpret_cast<char*>(&bone.giving_index), bone_index_size);
        ifs.read(reinterpret_cast<char*>(&bone.giving_rate), sizeof(float));
    }
    // axis fixed -> axis direction
    if(bone.flags & 0x0400) {
        ifs.read(reinterpret_cast<char*>(&bone.axis_dir), sizeof(glm::vec3));
    }
    // local axis -> x-axis and z-axis direction
    if(bone.flags & 0x0800) {
        ifs.read(reinterpret_cast<char*>(&bone.x_dir), sizeof(glm::vec3));
        ifs.read(reinterpret_cast<char*>(&bone.z_dir), sizeof(glm::vec3));
    }
    // external transform -> key value
    if(bone.flags & 0x2000) {
        ifs.read(reinterpret_cast<char*>(&bone.key), sizeof(uint32_t));
    }
    // IK -> IK information
    if(bone.flags & 0x0020) {
        // target index
        ifs.read(reinterpret_cast<char*>(&bone.ik_target_index), bone_index_size);
        // number of iteration
        ifs.read(reinterpret_cast<char*>(&bone.ik_iter_count), sizeof(uint32_t));
        // angle constraint
        ifs.read(reinterpret_cast<char*>(&bone.ik_constraint), sizeof(float));
        // number of IK links
        ifs.read(reinterpret_cast<char*>(&bone.ik_link_count), sizeof(uint32_t));
        bone.ik_link.resize(bone.ik_link_count);
        for(size_t i = 0; i < bone.ik_link.size(); ++i) {
            bone.ik_link[i] = std::move(read_ik_link(ifs, bone_index_size));
        }
    }

    return bone;
}

}

}