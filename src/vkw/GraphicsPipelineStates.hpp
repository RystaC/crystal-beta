#pragma once

#include "common.hpp"

namespace vkw {

class Device;

class GraphicsPipelineStates{
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
    VkPipelineVertexInputStateCreateInfo vertex_input_state_;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_;
    // VkPipelineTessellationStateCreateInfo tessellation_state_;
    VkPipelineViewportStateCreateInfo viewport_state_;
    VkPipelineRasterizationStateCreateInfo rasterization_state_;
    VkPipelineMultisampleStateCreateInfo multisample_state_;
    // VkPipelineDepthStencilStateCreateInfo depth_stencil_state_;
    std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states_;
    VkPipelineColorBlendStateCreateInfo color_blend_state_;
    VkPipelineDynamicStateCreateInfo dynamic_state_;

public:
    friend Device;

    GraphicsPipelineStates() noexcept :
        shader_stages_(),
        vertex_input_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO},
        input_assembly_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO},
        viewport_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO},
        rasterization_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO},
        multisample_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO},
        blend_attachment_states_(),
        color_blend_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO},
        dynamic_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO}
    {}

    auto& add_shader_stage(VkShaderStageFlagBits shader_stage, VkShaderModule shader_module, const char* entry_point) {
        shader_stages_.emplace_back(VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = shader_stage,
            .module = shader_module,
            .pName = entry_point,
        });

        return *this;
    }

    auto& vertex_input_state(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes) {
        vertex_input_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = size_u32(bindings.size()),
            .pVertexBindingDescriptions = bindings.data(),
            .vertexAttributeDescriptionCount = size_u32(attributes.size()),
            .pVertexAttributeDescriptions = attributes.data(),
        };

        return *this;
    }

    auto& input_assembly_state(VkPrimitiveTopology topology) {
        input_assembly_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = topology,
        };

        return *this;
    }

    auto& viewport_state(const std::vector<VkViewport>& viewports, const std::vector<VkRect2D>& scissors) {
        viewport_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = size_u32(viewports.size()),
            .pViewports = viewports.data(),
            .scissorCount = size_u32(scissors.size()),
            .pScissors = scissors.data(),
        };

        return *this;
    }

    auto& rasterization_state(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, float line_width) {
        rasterization_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = polygon_mode,
            .cullMode = cull_mode,
            .frontFace = front_face,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = line_width,
        };

        return *this;
    }

    auto& multisample_state(VkSampleCountFlagBits sample_count) {
        multisample_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = sample_count,
            .sampleShadingEnable = VK_FALSE,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        return *this;
    }

    auto& color_blend_state(uint32_t attachment_count) {
        blend_attachment_states_.resize(attachment_count, {.blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT,});
        color_blend_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = attachment_count,
            .pAttachments = blend_attachment_states_.data(),
        };

        return *this;
    }

    auto& dynamic_state(const std::vector<VkDynamicState>& dynamic_states) {
        dynamic_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = size_u32(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data(),
        };

        return *this;
    }

    // std::unique_ptr<Pipeline> create_pipeline(VkRenderPass render_pass, uint32_t subpass_index, VkPipelineLayout layout) {
    //     color_blend_state_.attachmentCount = size_u32(blend_attachment_states_.size());
    //     color_blend_state_.pAttachments = blend_attachment_states_.data();

    //     VkGraphicsPipelineCreateInfo pipeline_info {
    //         .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    //         .stageCount = size_u32(shader_stages_.size()),
    //         .pStages = shader_stages_.data(),
    //         .pVertexInputState = &vertex_input_state_,
    //         .pInputAssemblyState = &input_assembly_state_,
    //         .pViewportState = &viewport_state_,
    //         .pRasterizationState = &rasterization_state_,
    //         .pMultisampleState = &multisample_state_,
    //         .pColorBlendState = &color_blend_state_,
    //         .pDynamicState = &dynamic_state_,
    //         .layout = layout,
    //         .renderPass = render_pass,
    //         .subpass = subpass_index,
    //     };

    //     VkPipeline pipeline{};
    //     CHECK_VK_RESULT(vkCreateGraphicsPipelines(*device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline), return {}; );
        
    //     return std::make_unique<Pipeline>(device_, std::move(pipeline), std::move(layout));
    // }
};

}