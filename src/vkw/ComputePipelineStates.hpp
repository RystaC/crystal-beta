#pragma once

#include "common.hpp"
#include "Pipeline.hpp"

namespace vkw {

class ComputePipelineStates {
    std::shared_ptr<DeviceEntity> device_;
    VkPipelineShaderStageCreateInfo shader_stage_;

public:
    ComputePipelineStates(std::shared_ptr<DeviceEntity> device) noexcept :
        device_(device),
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

    std::unique_ptr<Pipeline> create_pipeline(VkPipelineLayout layout) {
        VkComputePipelineCreateInfo pipeline_info {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = shader_stage_,
            .layout = layout,
        };

        VkPipeline pipeline{};
        CHECK_VK_RESULT(vkCreateComputePipelines(*device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline), return {};);

        return std::make_unique<Pipeline>(device_, std::move(pipeline), std::move(layout));
    }

};

}