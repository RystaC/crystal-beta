#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class GraphicsPipelineStates;

class TessellationState {
    VkPipelineTessellationStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    TessellationState(uint32_t points) :
        state_(
            VkPipelineTessellationStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
                .patchControlPoints = points,
            }
        ) {}
};

}

}