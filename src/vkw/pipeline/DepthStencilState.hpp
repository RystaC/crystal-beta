#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class GraphicsPipelineStates;

class DepthStencilState {
    VkPipelineDepthStencilStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    DepthStencilState(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp compare_op, VkBool32 depth_bounds_test_enable, VkBool32 stencil_test_enable, VkStencilOp front = VK_STENCIL_OP_ZERO, VkStencilOp back = VK_STENCIL_OP_ZERO, float min_depth_bounds = 0.0f, float max_depth_bounds = 1.0f) :
        state_(
            VkPipelineDepthStencilStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = depth_test_enable,
                .depthWriteEnable = depth_write_enable,
                .depthCompareOp = compare_op,
                .depthBoundsTestEnable = depth_bounds_test_enable,
                .stencilTestEnable = stencil_test_enable,
                .front = front,
                .back = back,
                .minDepthBounds = min_depth_bounds,
                .maxDepthBounds = max_depth_bounds,
            }
        ) {}
};

}

}