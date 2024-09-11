#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class GraphicsPipelineStates;

class InputAssemblyState {
    VkPipelineInputAssemblyStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    InputAssemblyState(VkPrimitiveTopology topology) :
        state_(
            VkPipelineInputAssemblyStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = topology,
                .primitiveRestartEnable = VK_FALSE,
            }
        ) {}
};

}

}