#include "Obj.hpp"

namespace mesh {

Obj Obj::load(const std::filesystem::path& path) {
    std::ifstream ifs(path);
    if(ifs.fail()) throw std::runtime_error(std::format("[mesh::Obj::load] ERROR: failed to open {}.", path.string()));

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

    std::vector<VertexAttribute> interleaved(max_attribute_count);
    std::vector<uint32_t> indices(attribute_indices.size());

    for(size_t i = 0; i < attribute_indices.size(); ++i) {
        auto idx = (std::max)({attribute_indices[i].vertex, attribute_indices[i].tex_coord, attribute_indices[i].normal});
        interleaved[idx] = {
            .position = vertices[attribute_indices[i].vertex],
            .normal = attribute_indices[i].normal > -1 ? normals[attribute_indices[i].normal] : glm::vec3(0.0f),
            .tex_coord = attribute_indices[i].tex_coord > -1 ? tex_coords[attribute_indices[i].tex_coord] : glm::vec2(0.0f),
            .color = glm::vec4(1.0f),
        };
        indices[i] = idx;
    }

    return { std::move(interleaved), std::move(indices) };
}

void Obj::print_statistics() const {
    std::cerr << "# of vertex attributes = " << vertices_.size() << std::endl;
    std::cerr << "# of indices = " << indices_.size() << std::endl;

    for(size_t i = 0; i < indices_.size(); i += 3) {
        if(indices_[i+0] == indices_[i+1] || indices_[i+1] == indices_[i+2] || indices_[i+2] == indices_[i+0]) {
            std::cerr << std::format("invalid edge detected on face index {}: ({}, {}, {})", i, indices_[i+0], indices_[i+1], indices_[i+2]) << std::endl;
        }
    }
}

}