#include "PMX.hpp"

namespace mesh {

PMX PMX::load(const std::filesystem::path& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[mesh::PMX::load] ERROR: failed to open file: {}", path.string().c_str()));
    }

    // magic number "PMX "
    uint8_t magic[4];
    ifs.read(reinterpret_cast<char*>(magic), sizeof(uint8_t) * 4);
    auto magic_str = std::string_view((const char*)magic, 4);
    if(!(magic_str == "PMX ")) {
        throw std::runtime_error(std::format("[mesh::PMX::load] ERROR: input file is not PMX format: {}", path.string().c_str()));
    }

    PMX pmx{};

    // format version
    ifs.read(reinterpret_cast<char*>(&pmx.version_), sizeof(float));
    std::cerr << std::format("PMX version = {:.1f}", pmx.version_) << std::endl;

    // header size: must be 8
    uint8_t byte_size{};
    ifs.read(reinterpret_cast<char*>(&byte_size), sizeof(uint8_t));
    if(byte_size != 8) {
        throw std::runtime_error(std::format("[mesh::PMX::load] ERROR: wrong header byte size: {}", path.string().c_str()));
    }

    // header
    ifs.read(reinterpret_cast<char*>(&pmx.header_), sizeof(uint8_t) * 8);

    // model information
    pmx.name_ = pmx::read_text(ifs, pmx.header_.encode);
    pmx.name_en_ = pmx::read_text(ifs, pmx.header_.encode);
    pmx.comment_ = pmx::read_text(ifs, pmx.header_.encode);
    pmx.comment_en_ = pmx::read_text(ifs, pmx.header_.encode);
    std::cout << pmx.name_ << std::endl;
    std::cout << pmx.name_en_ << std::endl;
    std::cout << pmx.comment_ << std::endl;
    std::cout << pmx.comment_en_ << std::endl;

    // vertices
    uint32_t vertex_count{};
    ifs.read(reinterpret_cast<char*>(&vertex_count), sizeof(uint32_t));
    std::cout << std::format("# of vertices = {}" , vertex_count) << std::endl;
    pmx.vertices_.resize(vertex_count);
    for(size_t i = 0; i < pmx.vertices_.size(); ++i) {
        pmx.vertices_[i] = std::move(pmx::read_vertex(ifs, pmx.header_.additional_uv, pmx.header_.bone_index_size));
    }

    // index
    uint32_t index_count{};
    ifs.read(reinterpret_cast<char*>(&index_count), sizeof(uint32_t));
    std::cout << std::format("# of indices = {}", index_count) << std::endl;
    pmx.indices_.resize(index_count);
    for(size_t i = 0; i < pmx.indices_.size(); ++i) {
        ifs.read(reinterpret_cast<char*>(&pmx.indices_[i]), pmx.header_.vertex_index_size);
    }

    // textures
    uint32_t texture_count{};
    ifs.read(reinterpret_cast<char*>(&texture_count), sizeof(uint32_t));
    std::cout << std::format("# of textures = {}", texture_count) << std::endl;
    pmx.textures_.resize(texture_count);
    for(size_t i = 0; i < pmx.textures_.size(); ++i) {
        pmx.textures_[i] = std::move(pmx::read_text(ifs, pmx.header_.encode));
        std::cout << std::format("texture #{}: {}", i, pmx.textures_[i].string().c_str()) << std::endl;
    }

    // materials
    uint32_t material_count{};
    ifs.read(reinterpret_cast<char*>(&material_count), sizeof(uint32_t));
    std::cout << std::format("# of materials = {}", material_count) << std::endl;
    pmx.materials_.resize(material_count);
    for(size_t i = 0; i < pmx.materials_.size(); ++i) {
        pmx.materials_[i] = std::move(pmx::read_material(ifs, pmx.header_.texuture_index_size, pmx.header_.encode));
        std::cout << pmx.materials_[i].name << std::endl;
        std::cout << pmx.materials_[i].name_en << std::endl;
        std::cout << pmx.materials_[i].memo << std::endl;
    }

    // bones
    uint32_t bone_count{};
    ifs.read(reinterpret_cast<char*>(&bone_count), sizeof(uint32_t));
    std::cout << std::format("# of bones = {}", bone_count) << std::endl;
    pmx.bones_.resize(bone_count);
    for(size_t i = 0; i < pmx.bones_.size(); ++i) {
        pmx.bones_[i] = std::move(pmx::read_bone(ifs, pmx.header_.bone_index_size, pmx.header_.encode));
        std::cout << pmx.bones_[i].name << std::endl;
        std::cout << pmx.bones_[i].name_en << std::endl;
    }

    // morphs
    uint32_t morph_count{};
    ifs.read(reinterpret_cast<char*>(&morph_count), sizeof(uint32_t));
    std::cout << std::format("# of morphs = {}", morph_count) << std::endl;
    pmx.morphs_.resize(morph_count);
    for(size_t i = 0; i < pmx.morphs_.size(); ++i) {
        pmx.morphs_[i] = std::move(pmx::read_morph(ifs,
            pmx.header_.vertex_index_size, pmx.header_.bone_index_size,
            pmx.header_.material_index_size, pmx.header_.morph_index_size,
            pmx.header_.encode
        ));
        std::cout << pmx.morphs_[i].name << std::endl;
        std::cout << pmx.morphs_[i].name_en << std::endl;
    }

    // frames
    uint32_t frame_count{};
    ifs.read(reinterpret_cast<char*>(&frame_count), sizeof(uint32_t));
    std::cout << std::format("# of frames = {}", frame_count) << std::endl;
    pmx.frames_.resize(frame_count);
    for(size_t i = 0; i < pmx.frames_.size(); ++i) {
        pmx.frames_[i] = std::move(pmx::read_frame(ifs, pmx.header_.bone_index_size, pmx.header_.morph_index_size, pmx.header_.encode));
        std::cout << pmx.frames_[i].name << std::endl;
        std::cout << pmx.frames_[i].name_en << std::endl;
    }

    // rigids
    uint32_t rigid_count{};
    ifs.read(reinterpret_cast<char*>(&rigid_count), sizeof(uint32_t));
    std::cout << std::format("# of rigids = {}", rigid_count) << std::endl;
    pmx.rigids_.resize(rigid_count);
    for(size_t i = 0; i < pmx.rigids_.size(); ++i) {
        pmx.rigids_[i] = std::move(pmx::read_rigid(ifs, pmx.header_.bone_index_size, pmx.header_.encode));
        std::cout << pmx.rigids_[i].name << std::endl;
        std::cout << pmx.rigids_[i].name_en << std::endl;
        std::cout << std::format("topology: {}" , pmx.rigids_[i].topology) << std::endl;
    }

    // joints
    uint32_t joint_count{};
    ifs.read(reinterpret_cast<char*>(&joint_count), sizeof(uint32_t));
    std::cout << std::format("# of joints = {}", joint_count) << std::endl;
    pmx.joints_.resize(joint_count);
    for(size_t i = 0; i < pmx.joints_.size(); ++i) {
        pmx.joints_[i] = std::move(pmx::read_joint(ifs, pmx.header_.rigid_index_size, pmx.header_.encode));
        std::cout << pmx.joints_[i].name << std::endl;
        std::cout << pmx.joints_[i].name_en << std::endl;
    }

    return pmx;
}

}