#pragma once

#include <array>
#include <charconv>
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

    // just read
    static std::string_view read_(const std::string_view& str_view) {
        auto result_view = str_view.substr(0, str_view.find_first_of(' '));
        return result_view;
    }

    // return true if line has tokens
    // false otherwise
    static bool seek_(std::string_view& str_view) {
        if(str_view.find_first_of(' ') == std::string_view::npos) {
            return false;
        }

        str_view.remove_prefix(str_view.find_first_of(' '));
        str_view.remove_prefix(str_view.find_first_not_of(' '));

        return true;
    }

    static Obj::IndexLayout_ read_index_(std::string_view& str_view, size_t vertex_count, size_t texcoord_count, size_t normal_count) {
        auto v_str = str_view.substr(0, str_view.find_first_of('/'));

        str_view.remove_prefix(str_view.find_first_of('/') + 1);

        // v/t
        if(str_view.find_first_of('/') == std::string_view::npos) {
            int32_t v{}, t{};
            std::from_chars(v_str.data(), v_str.data() + v_str.length(), v);
            std::from_chars(str_view.data(), str_view.data() + str_view.length(), t);
            v = v < 0 ? static_cast<int32_t>(vertex_count) + v : v - 1;
            t = t < 0 ? static_cast<int32_t>(texcoord_count) + t : t - 1;
            
            return {v, t, -1};
        }
        // v//n
        else if(str_view.substr(0, str_view.find_first_of('/')).empty()) {
            str_view.remove_prefix(str_view.find_first_not_of('/'));

            int32_t v{}, n{};
            std::from_chars(v_str.data(), v_str.data() + v_str.length(), v);
            std::from_chars(str_view.data(), str_view.data() + str_view.length(), n);
            v = v < 0 ? static_cast<int32_t>(vertex_count) + v : v - 1;
            n = n < 0 ? static_cast<int32_t>(normal_count) + n : n - 1;

            return {v, -1, n};
        }
        // v/t/n
        else {
            auto t_str = str_view.substr(0, str_view.find_first_of('/'));

            str_view.remove_prefix(str_view.find_first_of('/') + 1);

            int32_t v{}, t{}, n{};
            std::from_chars(v_str.data(), v_str.data() + v_str.length(), v);
            std::from_chars(t_str.data(), t_str.data() + t_str.length(), t);
            std::from_chars(str_view.data(), str_view.data() + str_view.length(), n);
            v = v < 0 ? static_cast<int32_t>(vertex_count) + v : v - 1;
            t = t < 0 ? static_cast<int32_t>(texcoord_count) + t : t - 1;
            n = n < 0 ? static_cast<int32_t>(normal_count) + n : n - 1;

            return {v, t, n};
        }
    }

    static std::pair<std::vector<VertexAttribute>, std::vector<uint32_t>> make_interleaved_(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& texcoords, const std::vector<glm::vec3>& normals, const std::vector<IndexLayout_>& attribute_indices);

public:
    Obj(std::vector<VertexAttribute>&& vertices, std::vector<uint32_t>&& indices) noexcept : vertices_(vertices), indices_(indices) {}

    static Obj load(const char* path);

    const auto& vertices() const noexcept { return vertices_; }
    auto& vertices() noexcept { return vertices_; }
    const auto& indices() const noexcept { return indices_; }
    auto& indices() noexcept { return indices_; }

    void print_statistics() const;
};

}