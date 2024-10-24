#pragma once

#include <string_view>

#include <cstdio>

#include "common.hpp"

namespace mesh {

class Obj {
    std::vector<VertexAttribute> vertices_;
    std::vector<uint32_t> indices_;

    struct IndexLayout_ {
        int32_t vertex, tex_coord, normal;
    };

    static IndexLayout_ convert_index_(std::string& token, size_t vertex_count, size_t texture_count, size_t normal_count) {
        // [vertex position]
        if(token.find('/') == std::string::npos) {
            auto v_index = std::stoi(token);
            v_index = v_index < 0 ? static_cast<int32_t>(vertex_count) + v_index : v_index - 1;
            return { v_index, -1, -1 };
        }
        auto v_index = std::stoi(token.substr(0, token.find('/')));
        v_index = v_index < 0 ? static_cast<int32_t>(vertex_count) + v_index : v_index - 1;
        token = token.substr(token.find('/')+1);
        // [vertex position]/[texture coord]
        if(token.find('/') == std::string::npos) {
            auto t_index = std::stoi(token);
            t_index = t_index < 0 ? static_cast<int32_t>(texture_count) + t_index : t_index - 1;
            return { v_index, t_index, -1 };
        }
        // [vertex position]//[normal vector]
        else if(token.substr(0, token.find('/')).length() == 0) {
            token = token.substr(token.find('/')+1);
            auto n_index = std::stoi(token);
            n_index = n_index < 0 ? static_cast<int32_t>(normal_count) + n_index : n_index - 1;
            return { v_index, -1, n_index };
        }
        // [vertex position]/[texture coord]/[normal vector]
        else {
            auto t_index = std::stoi(token.substr(0, token.find('/')));
            t_index = t_index < 0 ? static_cast<int32_t>(texture_count) + t_index : t_index - 1;
            token = token.substr(token.find('/') + 1);
            auto n_index = std::stoi(token);
            n_index = n_index < 0 ? static_cast<int32_t>(normal_count) + n_index : n_index - 1;
            return { v_index, t_index, n_index };
        }
    }

public:
    Obj(std::vector<VertexAttribute>&& vertices, std::vector<uint32_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

    static Obj load(const std::filesystem::path& path);
    static Obj load_fast(const char* path);

    const auto& vertices() const noexcept { return vertices_; }
    auto& vertices() noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    auto& indices() noexcept { return indices_; }

    void print_statistics() const;
};

}