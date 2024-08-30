#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

class ComputePipelineStates {
    VkPipelineShaderStageCreateInfo shader_stage_;

public:
    friend Device;
    
    ComputePipelineStates() noexcept :
        shader_stage_{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}
    {}

    auto& shader_stage(VkShaderModule shader_module, const char* entry_point) {
        shader_stage_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shader_module,
            .pName = entry_point,
        };

        return *this;
    }

};

}