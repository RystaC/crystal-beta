#pragma once

#include <algorithm>
#include <filesystem>
#include <vector>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace images {

class Image {
    std::vector<glm::vec4> data_;
    VkFormat format_;

public:
    static Image load(const std::filesystem::path& file_path) {
        auto ext = file_path.extension();

        if(ext == ".jpg" || ext == ".jpeg") {

        }
        else if(ext == ".png") {

        }
        else if(ext == ".gif") {

        }
        else if(ext == ".bmp") {

        }
        else if(ext == ".dds") {

        }
        else {

        }
    }
};

}