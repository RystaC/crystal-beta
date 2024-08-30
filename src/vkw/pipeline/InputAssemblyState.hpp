#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class InputAssemblyState {
    VkPipelineInputAssemblyStateCreateInfo state_;

public:
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