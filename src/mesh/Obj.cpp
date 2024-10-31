#include "Obj.hpp"

namespace mesh {

Obj Obj::load(const char* path) {
    std::FILE* fp = std::fopen(path, "r");
    if(!fp) {
        throw std::runtime_error(std::format("[mesh::Obj::load] ERROR: failed to open {}.", path));
    }

    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> tex_coords{};
    std::vector<IndexLayout_> attribute_indices{};

    constexpr size_t BUF_SIZE = 256;

    std::array<char, BUF_SIZE> buf{};
    size_t line{};

    while(!std::feof(fp)) {
        // reset buffer
        buf.fill('\0');
        // get line
        std::fgets(buf.data(), static_cast<int>(buf.size()), fp);
        line++;
        // failed to read whole line -> error
        if(buf[BUF_SIZE-2] != '\0') {
            throw std::runtime_error(std::format("[mesh::Obj::load_fast] ERROR: buffer overflow. parser expects up to {} bytes for each line.", BUF_SIZE-1));
        }

        std::string_view str_view(buf.data());

        // line is not EOF -> remove line feed
        if(str_view.find('\n') != std::string_view::npos) {
            str_view.remove_suffix(1);
        }

        // empty line -> skip
        if(str_view.empty() || str_view.find_first_not_of(' ') == std::string_view::npos) {
            continue;
        }

        // remove front spaces
        str_view.remove_prefix(str_view.find_first_not_of(' '));
        // comment line -> skip
        if(str_view[0] == '#') {
            continue;
        }

        // extract first token
        auto header = read_(str_view);

        // vertex
        if(header == "v") {
            seek_(str_view);

            auto x_str = read_(str_view); seek_(str_view);
            auto y_str = read_(str_view); seek_(str_view);
            auto z_str = read_(str_view);
            float x{}, y{}, z{};
            std::from_chars(x_str.data(), x_str.data() + x_str.length(), x); std::from_chars(y_str.data(), y_str.data() + y_str.length(), y); std::from_chars(z_str.data(), z_str.data() + z_str.length(), z);
            vertices.emplace_back(glm::vec3(x, y, z));
        }
        // texcoord
        else if(header == "vt") {
            seek_(str_view);

            auto u_str = read_(str_view); seek_(str_view);
            auto v_str = read_(str_view);
            float u{}, v{};
            std::from_chars(u_str.data(), u_str.data() + u_str.length(), u); std::from_chars(v_str.data(), v_str.data() + v_str.length(), v);
            tex_coords.emplace_back(glm::vec2(u, v));
        }
        // normal
        else if(header == "vn") {
            seek_(str_view);

            auto x_str = read_(str_view); seek_(str_view);
            auto y_str = read_(str_view); seek_(str_view);
            auto z_str = read_(str_view);
            float x{}, y{}, z{};
            std::from_chars(x_str.data(), x_str.data() + x_str.length(), x); std::from_chars(y_str.data(), y_str.data() + y_str.length(), y); std::from_chars(z_str.data(), z_str.data() + z_str.length(), z);
            normals.emplace_back(glm::vec3(x, y, z));
        }
        // face
        else if(header == "f") {
            seek_(str_view);

            auto i0_str = read_(str_view); seek_(str_view);
            auto i1_str = read_(str_view); seek_(str_view);
            auto i2_str = read_(str_view);
            auto i0 = read_index_(i0_str, vertices.size(), tex_coords.size(), normals.size());
            auto i1 = read_index_(i1_str, vertices.size(), tex_coords.size(), normals.size());
            auto i2 = read_index_(i2_str, vertices.size(), tex_coords.size(), normals.size());
            attribute_indices.emplace_back(i0);
            attribute_indices.emplace_back(i1);
            attribute_indices.emplace_back(i2);

            // if(str_view.find_first_of(' ') != std::string_view::npos) {
            //     seek_(str_view);

            //     auto i3_str = read_(str_view);
            //     auto i3 = read_index_(i3_str, vertices.size(), tex_coords.size(), normals.size());
            //     attribute_indices.emplace_back(i3);
            //     attribute_indices.emplace_back(i0);
            //     attribute_indices.emplace_back(i2);
            // }
        }
    }
    std::fclose(fp);

    auto max_attribute_count = (std::max)({vertices.size(), tex_coords.size(), normals.size()});

    std::vector<VertexAttribute> interleaved{};
    std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> index_table{};
    std::vector<uint32_t> indices(attribute_indices.size());

    uint32_t idx = 0;
    for(size_t i = 0; i < attribute_indices.size(); ++i) {
        if(index_table.contains({attribute_indices[i].vertex, attribute_indices[i].tex_coord, attribute_indices[i].normal})) {
            indices[i] = index_table.at({attribute_indices[i].vertex, attribute_indices[i].tex_coord, attribute_indices[i].normal});
        }
        else {
            index_table[{attribute_indices[i].vertex, attribute_indices[i].tex_coord, attribute_indices[i].normal}] = idx;
            interleaved.emplace_back(
                VertexAttribute {
                    .position = vertices[attribute_indices[i].vertex],
                    .normal = normals[attribute_indices[i].normal],
                    .tex_coord = tex_coords[attribute_indices[i].tex_coord],
                    .color = glm::vec4(1.0f),
                }
            );
            indices[i] = idx;
            idx += 1;
        }
    }

    return { std::move(interleaved), std::move(indices) };
}

void Obj::print_statistics() const {
    std::cerr << std::format("# of vertex attributes = {}", vertices_.size()) << std::endl;
    std::cerr << std::format("# of indices = {}", indices_.size()) << std::endl;

    for(size_t i = 0; i < indices_.size(); i += 3) {
        if(indices_[i+0] == indices_[i+1] || indices_[i+1] == indices_[i+2] || indices_[i+2] == indices_[i+0]) {
            std::cerr << std::format("invalid edge detected on face index {}: ({}, {}, {})", i, indices_[i+0], indices_[i+1], indices_[i+2]) << std::endl;
        }
    }
}

}