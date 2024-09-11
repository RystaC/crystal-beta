#pragma once

#include "../common/common.hpp"

#include "ShaderStage.hpp"

namespace vkw {

class Device;

namespace pipeline {

class ComputePipelineStates {
    VkComputePipelineCreateInfo pipeline_info_;

public:
    friend vkw::Device;

    ComputePipelineStates(const ComputeShaderStage& stage) noexcept : pipeline_info_{.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, .stage = stage.shader_stage_} {}
};

}

}