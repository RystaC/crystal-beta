#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class RasterizarionState {
    VkPipelineRasterizationStateCreateInfo state_;

public:
    RasterizarionState(VkBool32 depth_clamp_enable, VkBool32 discard_enable, VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, float line_width, VkBool32 depth_bias_enable = VK_FALSE, float depth_bias_constant_factor = 0.0f, float depth_bias_clamp = 0.0f, float depth_bias_slope_factor = 0.0f) :
        state_(
            VkPipelineRasterizationStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = depth_clamp_enable,
                .rasterizerDiscardEnable = discard_enable,
                .polygonMode = polygon_mode,
                .cullMode = cull_mode,
                .frontFace = front_face,
                .depthBiasEnable = depth_bias_enable,
                .depthBiasConstantFactor = depth_bias_constant_factor,
                .depthBiasClamp = depth_bias_clamp,
                .depthBiasSlopeFactor = depth_bias_slope_factor,
                .lineWidth = line_width,
            }
        ) {}
};

}

}