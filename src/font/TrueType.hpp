#pragma once

#include "common.hpp"
#include "true_type/Head.hpp"
#include "true_type/Maxp.hpp"
#include "true_type/Loca.hpp"
#include "true_type/Cmap.hpp"
#include "true_type/Glyf.hpp"

namespace font {

class TrueType {
    using ShortFrac_ = int16_t;
    using Fixed_ = int32_t;
    using FWord_ = int16_t;
    using UFWord_ = uint16_t;
    using F2Dot14_ = int16_t;
    using LongDateTime_ = int64_t;


    template<typename T>
    static void read_be_(std::ifstream& ifs, T& dst) {
        if constexpr (sizeof(T) == 1) {
            uint8_t bytes{};
            ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));
            dst = std::bit_cast<T, uint8_t>(bytes);
        }
        else if constexpr(sizeof(T) == 2) {
            uint16_t bytes{};
            ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

            bytes = (((bytes >> 000) & 0xff) << 010) |
                    (((bytes >> 010) & 0xff) << 000);

            dst = std::bit_cast<T, uint16_t>(bytes);
        }
        else if constexpr(sizeof(T) == 4) {
            uint32_t bytes{};
            ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

            bytes = (((bytes >> 000) & 0xff) << 030) |
                    (((bytes >> 010) & 0xff) << 020) |
                    (((bytes >> 020) & 0xff) << 010) |
                    (((bytes >> 030) & 0xff) << 000);

            dst = std::bit_cast<T, uint32_t>(bytes);
        }
        else if constexpr(sizeof(T) == 8) {
            uint64_t bytes{};
            ifs.read(reinterpret_cast<char*>(&bytes), sizeof(T));

            bytes = (((bytes >> 000) & 0xff) << 070) |
                    (((bytes >> 010) & 0xff) << 060) |
                    (((bytes >> 020) & 0xff) << 050) |
                    (((bytes >> 030) & 0xff) << 040) |
                    (((bytes >> 040) & 0xff) << 030) |
                    (((bytes >> 050) & 0xff) << 020) |
                    (((bytes >> 060) & 0xff) << 010) |
                    (((bytes >> 070) & 0xff) << 000);

            dst = std::bit_cast<T, uint64_t>(bytes);
        }
        else {
            static_assert([]{ return false; }, "[font::TrueType::read_be_] ERROR: unexpeced byte size");
        }
    }

    struct TableDirectory_ {
        uint32_t check_sum;
        uint32_t offset;
        uint32_t length;
    };

    static std::pair<std::string, TableDirectory_> read_table_directory_(std::ifstream& ifs) {
        std::string tag(4, '\0');
        TableDirectory_ td{};
        read_be_(ifs, tag[0]);
        read_be_(ifs, tag[1]);
        read_be_(ifs, tag[2]);
        read_be_(ifs, tag[3]);
        read_be_(ifs, td.check_sum);
        read_be_(ifs, td.offset);
        read_be_(ifs, td.length);

        return {tag, td};
    }

    true_type::Head head_;
    true_type::Maxp maxp_;
    true_type::cmap::Format12 char_map_;
    std::vector<true_type::Glyf> glyphs_;

public:
    static TrueType load(const std::filesystem::path& path);

    std::pair<std::vector<glm::vec2>, std::vector<uint16_t>> glyph(uint32_t char_code) {
        std::vector<glm::vec2> vertices{};
        std::vector<uint16_t> indices{};

        auto glyph_index = char_map_.glyph_index(char_code);
        auto& glyph = glyphs_[glyph_index];

        auto scale = glm::vec2(float(head_.x_max - head_.x_min), float(head_.y_max - head_.y_min));

        uint16_t first_point_index = 0;
        for(size_t i = 0; i < glyph.end_pts_of_contours.size(); ++i) {
            for(uint16_t j = 0; first_point_index + j <= glyph.end_pts_of_contours[i]; ++j) {
                auto point = glm::vec2(float(glyph.x_coordinates[first_point_index + j] - head_.x_min) / scale.x, float(glyph.y_coordinates[first_point_index + j] - head_.y_min) / scale.y);
                vertices.push_back(point);
                auto p0_index = first_point_index + j;
                auto p1_index = first_point_index + j + 1 > glyph.end_pts_of_contours[i] ? first_point_index : first_point_index + j + 1;
                indices.push_back(p0_index);
                indices.push_back(p1_index);
            }
            first_point_index = glyph.end_pts_of_contours[i] + 1;
        }

        return {vertices, indices};
    }

    std::vector<std::pair<glm::vec2, glm::vec4>> point_vertices(uint32_t char_code) {
        std::vector<std::pair<glm::vec2, glm::vec4>> points{};
        auto glyph_index = char_map_.glyph_index(char_code);
        auto& glyph = glyphs_[glyph_index];

        auto scale = glm::vec2(float(head_.x_max - head_.x_min), float(head_.y_max - head_.y_min));

        for(size_t i = 0; i < glyph.flags.size(); ++i) {
            auto point = glm::vec2(float(glyph.x_coordinates[i] - head_.x_min) / scale.x, float(glyph.y_coordinates[i] - head_.y_min) / scale.y);
            // on curve point
            if(glyph.flags[i] & 0x01) {
                points.push_back({point, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)});
            }
            // off curve point
            else {
                points.push_back({point, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)});
            }
        }

        return points;
    }
};

}