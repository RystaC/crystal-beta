#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class ViewportState {
    VkPipelineViewportStateCreateInfo state_;

public:
    ViewportState(const std::vector<VkViewport>& viewports, const std::vector<VkRect2D>& scissors) :
        state_(
            VkPipelineViewportStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = size_u32(viewports.size()),
                .pViewports = viewports.data(),
                .scissorCount = size_u32(scissors.size()),
                .pScissors = scissors.data(),
            }
        ) {}
};

}

}