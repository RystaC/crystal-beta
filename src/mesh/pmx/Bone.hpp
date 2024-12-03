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
    link.index = read_index(ifs, bone_index_size);
    // angle constraint
    ifs.read(reinterpret_cast<char*>(&link.is_limited), sizeof(uint8_t));
    if(link.is_limited) {
        // lower bound
        ifs.read(reinterpret_cast<char*>(&link.lower_bound), sizeof(glm::vec3));
        ifs.read(reinterpret_cast<char*>(&link.upper_bound), sizeof(glm::vec3));
    }

    return link;
}

struct IK {
    int32_t index;
    uint32_t loop_count;
    float constraint_rad;
    std::vector<IKLink> links;
};

inline IK read_ik(std::ifstream& ifs, uint8_t bone_index_size) {
    IK ik{};

    // target bone index
    ik.index = read_index(ifs, bone_index_size);
    // loop count
    ifs.read(reinterpret_cast<char*>(&ik.loop_count), sizeof(uint32_t));
    // constraint
    ifs.read(reinterpret_cast<char*>(&ik.constraint_rad), sizeof(float));
    // link count
    uint32_t link_count{};
    ifs.read(reinterpret_cast<char*>(&link_count), sizeof(uint32_t));
    ik.links.resize(link_count);
    // links
    for(size_t i = 0; i < ik.links.size(); ++i) {
        ik.links[i] = std::move(read_ik_link(ifs, bone_index_size));
    }

    return ik;
}

struct Bone {
    glm::vec3 position;
    int32_t parent_index;
    uint32_t hierarchy;
    uint16_t flags;
    uint32_t giving_index;
    float giving_rate;
    uint32_t key;
    IK ik;

    // display/control data
    union Connect {
        glm::vec3 offset;
        int32_t dst_index;
    } connect;
    glm::vec3 axis_dir;
    glm::vec3 local_x_axis;
    glm::vec3 local_z_axis;

    // meta data
    std::filesystem::path name;
    std::filesystem::path name_en;
};

inline Bone read_bone(std::ifstream& ifs, uint8_t bone_index_size, bool is_utf8) {
    Bone bone{};

    // name
    bone.name = std::move(read_text(ifs, is_utf8));
    bone.name_en = std::move(read_text(ifs, is_utf8));

    // position
    ifs.read(reinterpret_cast<char*>(&bone.position), sizeof(glm::vec3));
    // parent index
    bone.parent_index = read_index(ifs, bone_index_size);
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
        bone.connect.dst_index = read_index(ifs, bone_index_size);
    }
    // rotate | translate giving -> index and giving rate
    if((bone.flags & 0x0100) || (bone.flags & 0x0200)) {
        bone.giving_index = read_index(ifs, bone_index_size);
        ifs.read(reinterpret_cast<char*>(&bone.giving_rate), sizeof(float));
    }
    // axis fixed -> axis direction
    if(bone.flags & 0x0400) {
        ifs.read(reinterpret_cast<char*>(&bone.axis_dir), sizeof(glm::vec3));
    }
    // local axis -> x-axis and z-axis direction
    if(bone.flags & 0x0800) {
        ifs.read(reinterpret_cast<char*>(&bone.local_x_axis), sizeof(glm::vec3));
        ifs.read(reinterpret_cast<char*>(&bone.local_z_axis), sizeof(glm::vec3));
    }
    // external transform -> key value
    if(bone.flags & 0x2000) {
        ifs.read(reinterpret_cast<char*>(&bone.key), sizeof(uint32_t));
    }
    // IK -> IK information
    if(bone.flags & 0x0020) {
        bone.ik = std::move(read_ik(ifs, bone_index_size));
    }

    return bone;
}

}

}