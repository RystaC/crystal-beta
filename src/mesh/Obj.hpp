#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace mesh {

class Obj {
    std::vector<glm::vec3> vertices_;
    std::vector<uint32_t> indices_;

    static uint32_t convert_index_(const std::string& token, size_t current_vertex_count) {
        int32_t index = std::stoi(token.substr(0, token.find('/')));
        return index < 0 ? current_vertex_count + index : index - 1;
    }

public:
    Obj(std::vector<glm::vec3>&& vertices, std::vector<uint32_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

    static Obj load(const std::filesystem::path& path);
};

}