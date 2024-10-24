#include "Obj.hpp"

namespace mesh {

Obj Obj::load(const std::filesystem::path& path) {
    std::ifstream ifs(path);
    if(ifs.fail()) {
        throw std::runtime_error(std::format("[mesh::Obj::load] ERROR: failed to open {}.", path.string()));
    }

    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> tex_coords{};
    std::vector<IndexLayout_> attribute_indices{};

    while(!ifs.eof()) {
        {
            std::vector<std::string> line{};
            {
                std::string str{};
                std::getline(ifs, str);
                std::istringstream iss(str);
                std::string token;
                while(std::getline(iss, token, ' ')) {
                    if(!token.empty()) {
                        line.emplace_back(token);
                    }
                }
            }

            if(line.empty()) continue;
            // vertex
            else if(line[0] == "v") {
                // extract x, y, z (ignore w)
                vertices.emplace_back(glm::vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])));
            }
            // tex coord
            else if(line[0] == "vt") {
                // extract u, v (ignore w)
                tex_coords.emplace_back(glm::vec2(std::stof(line[1]), std::stof(line[2])));
            }
            // normal
            else if(line[0] == "vn") {
                // extract x, y, z
                normals.emplace_back(glm::vec3(std::stof(line[1]), std::stof(line[2]), std::stof(line[3])));
            }
            // face
            else if(line[0] == "f") {
                if(line.size() == 4) {
                    attribute_indices.emplace_back(convert_index_(line[1], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[2], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[3], vertices.size(), tex_coords.size(), normals.size()));
                }
                else if(line.size() == 5) {
                    attribute_indices.emplace_back(convert_index_(line[1], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[2], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[3], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[4], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[1], vertices.size(), tex_coords.size(), normals.size()));
                    attribute_indices.emplace_back(convert_index_(line[3], vertices.size(), tex_coords.size(), normals.size()));
                }
                else {
                    ifs.close();
                    throw std::runtime_error(std::format("[mesh::Obj::load] ERROR: face should have 3 or 4 elements. but {} elements found.", line.size() - 1));
                }
            }
        }
    }

    ifs.close();

    auto max_attribute_count = (std::max)({vertices.size(), tex_coords.size(), normals.size()});

    std::vector<VertexAttribute> interleaved{};
    std::vector<uint32_t> indices(attribute_indices.size());

    std::map<std::tuple<int32_t, int32_t, int32_t>, uint32_t> index_table{};

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

enum class ParseMode {
    HEADER,
    VERTEX,
    NORMAL,
    TEXCOORD,
    INDEX,
    END,
};

enum class ParseModeTuple {
    X,
    Y,
    Z,
};

enum class ParseModeIndex {
    VERTEX,
    TEXCOORD,
    NORMAL,
};

Obj Obj::load_fast(const char* path) {
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

    while(!std::feof(fp)) {
        // reset buffer
        buf.fill('\0');
        // get line
        std::fgets(buf.data(), buf.size(), fp);
        // failed to read whole line -> error
        if(buf[BUF_SIZE-2] != '\0') {
            throw std::runtime_error(std::format("[mesh::Obj::load_fast] ERROR: buffer overflow. parser expects up to {} bytes for each line.", BUF_SIZE-1));
        }

        std::string_view str_view(buf.data());
        // remove front spaces
        str_view.remove_prefix(str_view.find_first_not_of(' '));
        // comment -> skip line
        if(str_view[0] == '#') {
            continue;
        }

        // extract first token
        auto header = str_view.substr(0, str_view.find_first_of(' '));
        str_view = str_view.substr(str_view.find_first_not_of(' '));
        // vertex
        if(header == "v") {
            auto x_str = str_view.substr(0, str_view.find_first_of(' '));
            str_view = str_view.substr(str_view.find_first_not_of(' '));
            auto y_str = str_view.substr(0, str_view.find_first_of(' '));
            str_view = str_view.substr(str_view.find_first_not_of(' '));
            auto z_str = str_view.substr(0, str_view.find_first_of(' '));
            float x{}, y{}, z{};
            std::from_chars(x_str.begin(), x_str.end(), x);
            std::from_chars(y_str.begin(), y_str.end(), y);
            std::from_chars(z_str.begin(), z_str.end(), z);
            vertices.emplace_back(glm::vec3(x, y, z));
        }
        // texcoord
        else if(header == "vt") {
            auto u_str = str_view.substr(0, str_view.find_first_of(' '));
            str_view = str_view.substr(str_view.find_first_not_of(' '));
            auto v_str = str_view.substr(0, str_view.find_first_of(' '));
            float u{}, v{};
            std::from_chars(u_str.begin(), u_str.end(), u);
            std::from_chars(v_str.begin(), v_str.end(), v);
            vertices.emplace_back(glm::vec2(u, v));
        }
        // normal
        else if(header == "vn") {
            auto x_str = str_view.substr(0, str_view.find_first_of(' '));
            str_view = str_view.substr(str_view.find_first_not_of(' '));
            auto y_str = str_view.substr(0, str_view.find_first_of(' '));
            str_view = str_view.substr(str_view.find_first_not_of(' '));
            auto z_str = str_view.substr(0, str_view.find_first_of(' '));
            float x{}, y{}, z{};
            std::from_chars(x_str.begin(), x_str.end(), x);
            std::from_chars(y_str.begin(), y_str.end(), y);
            std::from_chars(z_str.begin(), z_str.end(), z);
            normals.emplace_back(glm::vec3(x, y, z));
        }
        // face
        else if(header == "f") {

        }
    }
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