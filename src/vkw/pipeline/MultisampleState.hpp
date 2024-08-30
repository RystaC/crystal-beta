#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class MultisampleState {
    VkPipelineMultisampleStateCreateInfo state_;

public:
    MultisampleState(VkSampleCountFlagBits samples, VkBool32 sample_shading_enable = VK_FALSE, float min_sample_shading = 0.0f, const std::optional<std::vector<VkSampleMask>>& sample_mask = std::nullopt, VkBool32 alpha_to_coverage_enable = VK_FALSE, VkBool32 alpha_to_one_enable = VK_FALSE) :
        state_(
            VkPipelineMultisampleStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = samples,
                .sampleShadingEnable = sample_shading_enable,
                .minSampleShading = min_sample_shading,
                .pSampleMask = sample_mask.has_value() ? sample_mask.value().data() : nullptr,
                .alphaToCoverageEnable = alpha_to_coverage_enable,
                .alphaToOneEnable = alpha_to_one_enable,
            }
        ) {}
};

}

}