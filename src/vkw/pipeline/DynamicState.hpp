#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class DynamicState {
    VkPipelineDynamicStateCreateInfo state_;

public:
    DynamicState(const std::vector<VkDynamicState>& dynamic_states) :
        state_(
            VkPipelineDynamicStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = size_u32(dynamic_states.size()),
                .pDynamicStates = dynamic_states.data(),
            }
        ) {}
};

}

}