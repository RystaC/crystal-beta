#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class GraphicsPipelineStates;

class ViewportState {
    VkPipelineViewportStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    // TODO: support multi viewport feature
    ViewportState(const VkViewport& viewport, const VkRect2D& scissor) :
        state_(
            VkPipelineViewportStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .pViewports = &viewport,
                .scissorCount = 1,
                .pScissors = &scissor,
            }
        ) {}
};

}

}