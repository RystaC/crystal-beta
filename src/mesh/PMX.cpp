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

    // vertices
    uint32_t vertex_count{};
    ifs.read(reinterpret_cast<char*>(&vertex_count), sizeof(uint32_t));
    pmx.vertices_.resize(vertex_count);
    for(size_t i = 0; i < pmx.vertices_.size(); ++i) {
        pmx.vertices_[i] = std::move(pmx::read_vertex(ifs, pmx.header_.additional_uv, pmx.header_.bone_index_size));
    }

    // index
    uint32_t index_count{};
    ifs.read(reinterpret_cast<char*>(&index_count), sizeof(uint32_t));
    pmx.indices_.resize(index_count);
    for(size_t i = 0; i < pmx.indices_.size(); ++i) {
        pmx.indices_[i] = pmx::read_index(ifs, pmx.header_.vertex_index_size);
    }

    // textures
    uint32_t texture_count{};
    ifs.read(reinterpret_cast<char*>(&texture_count), sizeof(uint32_t));
    pmx.textures_.resize(texture_count);
    for(size_t i = 0; i < pmx.textures_.size(); ++i) {
        pmx.textures_[i] = std::move(pmx::read_text(ifs, pmx.header_.encode));
    }

    // materials
    uint32_t material_count{};
    ifs.read(reinterpret_cast<char*>(&material_count), sizeof(uint32_t));
    pmx.materials_.resize(material_count);
    for(size_t i = 0; i < pmx.materials_.size(); ++i) {
        pmx.materials_[i] = std::move(pmx::read_material(ifs, pmx.header_.texuture_index_size, pmx.header_.encode));
    }

    // bones
    uint32_t bone_count{};
    ifs.read(reinterpret_cast<char*>(&bone_count), sizeof(uint32_t));
    pmx.bones_.resize(bone_count);
    for(size_t i = 0; i < pmx.bones_.size(); ++i) {
        pmx.bones_[i] = std::move(pmx::read_bone(ifs, pmx.header_.bone_index_size, pmx.header_.encode));
    }

    // morphs
    uint32_t morph_count{};
    ifs.read(reinterpret_cast<char*>(&morph_count), sizeof(uint32_t));
    pmx.morphs_.resize(morph_count);
    for(size_t i = 0; i < pmx.morphs_.size(); ++i) {
        pmx.morphs_[i] = std::move(pmx::read_morph(ifs,
            pmx.header_.vertex_index_size, pmx.header_.bone_index_size,
            pmx.header_.material_index_size, pmx.header_.morph_index_size,
            pmx.header_.encode
        ));
    }

    // frames
    uint32_t frame_count{};
    ifs.read(reinterpret_cast<char*>(&frame_count), sizeof(uint32_t));
    pmx.frames_.resize(frame_count);
    for(size_t i = 0; i < pmx.frames_.size(); ++i) {
        pmx.frames_[i] = std::move(pmx::read_frame(ifs, pmx.header_.bone_index_size, pmx.header_.morph_index_size, pmx.header_.encode));
    }

    // rigids
    uint32_t rigid_count{};
    ifs.read(reinterpret_cast<char*>(&rigid_count), sizeof(uint32_t));
    pmx.rigids_.resize(rigid_count);
    for(size_t i = 0; i < pmx.rigids_.size(); ++i) {
        pmx.rigids_[i] = std::move(pmx::read_rigid(ifs, pmx.header_.bone_index_size, pmx.header_.encode));
    }

    // joints
    uint32_t joint_count{};
    ifs.read(reinterpret_cast<char*>(&joint_count), sizeof(uint32_t));
    pmx.joints_.resize(joint_count);
    for(size_t i = 0; i < pmx.joints_.size(); ++i) {
        pmx.joints_[i] = std::move(pmx::read_joint(ifs, pmx.header_.rigid_index_size, pmx.header_.encode));
    }

    return pmx;
}

void PMX::print_model_info() const {
    std::cout << "name: " << name_ << "\n";
    std::cout << "name (en): " << name_en_ << "\n";
    std::cout << "comment: " << comment_ << "\n";
    std::cout << "comment (en): " << comment_en_ << "\n";
    std::cout << std::format("version: {}\nencode: {}\n# of additional UV: {}\nvertex index size: {}\ntexture index size: {}\n", version_, header_.encode == 0 ? "UTF-16" : "UTF-8", header_.additional_uv, header_.vertex_index_size, header_.texuture_index_size);
    std::cout << std::format("material index size: {}\nbone index size: {}\nmorph index size: {}\nrigid index size: {}\n", header_.material_index_size, header_.bone_index_size, header_.morph_index_size, header_.rigid_index_size) << std::endl;
}

void PMX::print_mesh_info() const {
    std::cout << std::format("# of vertices = {}\n# of indices = {}", vertices_.size(), indices_.size()) << std::endl;
}

void PMX::print_texture_info() const {
    for(size_t i = 0; i < textures_.size(); ++i) {
        std::cout << "texture #" << i << ": " << textures_[i] << "\n";
    }
    std::cout << std::endl;
}

void PMX::print_material_info() const {
    for(size_t i = 0; i < materials_.size(); ++i) {
        std::cout << "material #" << i << "\nname: " << materials_[i].name << "\nname (en): " << materials_[i].name_en << "\nmemo: " << materials_[i].memo << "\n";
        std::cout << std::format("diffuse: {}\nspecular: {}, intensity: {}\nambient: {}\nedge color: {}, size: {}\n", materials_[i].diffuse, materials_[i].specular, materials_[i].specular_intensity, materials_[i].ambient, materials_[i].edge_color, materials_[i].edge_size);
        std::cout << std::format("texture index: {}, sphere index: {}, toon index: {}\n# of indices: {}\n\n", materials_[i].texture_indices.x, materials_[i].texture_indices.y, materials_[i].texture_indices.z, materials_[i].vertex_count);
    }
    std::cout << std::endl;
}

void PMX::print_bone_info() const {
    for(size_t i = 0; i < bones_.size(); ++i) {
        std::cout << "bone #" << i << "\nname: " << bones_[i].name << "\nname (en): " << bones_[i].name_en << "\n";
        std::cout << std::format("position: {}\nparent index: {}, hierarchy: {}\n", bones_[i].position, bones_[i].parent_index, bones_[i].hierarchy);
        std::cout << std::format("connectivity: {}\nrotatable: {}, translatable: {}, display: {}, controlable: {}\n", bones_[i].flags & 0x0001 ? "offset" : "bone", bool(bones_[i].flags & 0x0002), bool(bones_[i].flags & 0x0004), bool(bones_[i].flags & 0x0008), bool(bones_[i].flags & 0x0010));
        std::cout << std::format("IK: {}, local giving: {}, rotate giving: {}, translate giving: {}\n", bool(bones_[i].flags & 0x0020), bool(bones_[i].flags & 0x0080), bool(bones_[i].flags & 0x0100), bool(bones_[i].flags & 0x0200));
        std::cout << std::format("fixed axis: {}, local axis: {}, before physics: {}, external parent: {}\n", bool(bones_[i].flags & 0x0400), bool(bones_[i].flags & 0x0800), bool(bones_[i].flags & 0x1000), bool(bones_[i].flags & 0x2000));
    }
}

void PMX::print_morph_info() const {

}

void PMX::print_rigid_info() const {

}

void PMX::print_joint_info() const {

}

}