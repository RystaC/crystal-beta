#pragma once

#include "common.hpp"

#include "pmx/utils.hpp"
#include "pmx/Vertex.hpp"
#include "pmx/Material.hpp"
#include "pmx/Bone.hpp"
#include "pmx/Morph.hpp"
#include "pmx/Frame.hpp"
#include "pmx/Rigid.hpp"
#include "pmx/Joint.hpp"

namespace mesh {

class PMX {
    float version_;
    struct Header_ {
        uint8_t encode;
        uint8_t additional_uv;
        uint8_t vertex_index_size;
        uint8_t texuture_index_size;
        uint8_t material_index_size;
        uint8_t bone_index_size;
        uint8_t morph_index_size;
        uint8_t rigid_index_size;
    } header_;

    std::filesystem::path name_;
    std::filesystem::path name_en_;
    std::filesystem::path comment_;
    std::filesystem::path comment_en_;

    std::vector<pmx::Vertex> vertices_;
    std::vector<uint32_t> indices_;
    std::vector<std::filesystem::path> textures_;
    std::vector<pmx::Material> materials_;
    std::vector<pmx::Bone> bones_;
    std::vector<pmx::Morph> morphs_;
    std::vector<pmx::Frame> frames_;
    std::vector<pmx::Rigid> rigids_;
    std::vector<pmx::Joint> joints_;

public:
    static PMX load(const std::filesystem::path& path);

    const auto& vertices() const noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    const auto& textures() const noexcept { return textures_; }
    const auto& materials() const noexcept { return materials_; }
    const auto& bones() const noexcept { return bones_; }
    const auto& morphs() const noexcept { return morphs_; }
    const auto& frames() const noexcept { return frames_; }
    const auto& rigids() const noexcept { return rigids_; }
    const auto& joints() const noexcept { return joints_; }
};

}