#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace pipeline_layout {

class CreateInfo {
    std::vector<VkDescriptorSetLayout> layouts_;
    std::vector<VkPushConstantRange> ranges_;

public:
    friend vkw::Device;

    auto& add_descriptor_set_layout(VkDescriptorSetLayout layout) {
        layouts_.emplace_back(layout);

        return *this;
    }

    auto& add_push_constant_range(VkShaderStageFlags shader_stage, uint32_t offset, uint32_t size) {
        ranges_.emplace_back(
            VkPushConstantRange {
                .stageFlags = shader_stage,
                .offset = offset,
                .size = size
            }
        );

        return *this;
    }
};

}

}