#include "Obj.hpp"

namespace mesh {

Obj Obj::load(const std::filesystem::path& path) {
    std::ifstream ifs(path);
    if(ifs.fail()) throw std::runtime_error("failed to open .obj file.");

    std::vector<glm::vec3> vertices{};
    std::vector<uint32_t> indices{};

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
            else if(line[0] == "v") {
                vertices.emplace_back(glm::vec3( std::stof(line[1]), std::stof(line[2]), std::stof(line[3]) ));
            }
            else if(line[0] == "f") {
                if(line.size() == 4) {
                    indices.emplace_back(convert_index_(line[1], vertices.size()));
                    indices.emplace_back(convert_index_(line[2], vertices.size()));
                    indices.emplace_back(convert_index_(line[3], vertices.size()));
                }
                else if(line.size() == 5) {
                    indices.emplace_back(convert_index_(line[1], vertices.size()));
                    indices.emplace_back(convert_index_(line[2], vertices.size()));
                    indices.emplace_back(convert_index_(line[3], vertices.size()));
                    indices.emplace_back(convert_index_(line[4], vertices.size()));
                    indices.emplace_back(convert_index_(line[1], vertices.size()));
                    indices.emplace_back(convert_index_(line[3], vertices.size()));
                }
            }
        }
    }

    return { std::move(vertices), std::move(indices) };
}

}