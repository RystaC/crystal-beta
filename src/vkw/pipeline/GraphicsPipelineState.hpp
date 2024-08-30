#pragma once

#include "common.hpp"

namespace vkw {

class GraphicsPipelineStates;

class VertexInputBindingDescriptions {
    std::vector<VkVertexInputBindingDescription> descs_;

public:
    friend GraphicsPipelineStates;

    auto& add(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate) {
        descs_.emplace_back(
            VkVertexInputBindingDescription {
                .binding = binding,
                .stride = stride,
                .inputRate = input_rate,
            }
        );
        return *this;
    }
};

class VertexInputAttributeDescriptions {
    std::vector<VkVertexInputAttributeDescription> descs_;

public:
    friend GraphicsPipelineStates;
    auto& add(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
        descs_.emplace_back(
            VkVertexInputAttributeDescription {
                .location = location,
                .binding = binding,
                .format = format,
                .offset = offset,
            }
        );

        return *this;
    }
};

class Device;

class GraphicsPipelineStates{
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
    VkPipelineVertexInputStateCreateInfo vertex_input_state_;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_;
    // VkPipelineTessellationStateCreateInfo tessellation_state_;
    VkPipelineViewportStateCreateInfo viewport_state_;
    VkPipelineRasterizationStateCreateInfo rasterization_state_;
    VkPipelineMultisampleStateCreateInfo multisample_state_;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_;
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
        depth_stencil_state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO},
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

    auto& vertex_input_state(const VertexInputBindingDescriptions& bindings, const VertexInputAttributeDescriptions& attributes) {
        vertex_input_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = size_u32(bindings.descs_.size()),
            .pVertexBindingDescriptions = bindings.descs_.data(),
            .vertexAttributeDescriptionCount = size_u32(attributes.descs_.size()),
            .pVertexAttributeDescriptions = attributes.descs_.data(),
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

    auto& depth_stencil_state(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op, VkBool32 depth_bounds_test_enable, VkBool32 stencil_test_enable, const VkStencilOpState& front = {}, const VkStencilOpState& back = {}, float min_depth_bound = 0.0f, float max_depth_bound = 1.0f) {
        depth_stencil_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = depth_test_enable,
            .depthWriteEnable = depth_write_enable,
            .depthCompareOp = depth_compare_op,
            .depthBoundsTestEnable = depth_bounds_test_enable,
            .stencilTestEnable = stencil_test_enable,
            .front = front,
            .back = back,
            .minDepthBounds = min_depth_bound,
            .maxDepthBounds = max_depth_bound,
        };

        return *this;
    }

    auto& color_blend_state(const std::vector<VkPipelineColorBlendAttachmentState>& blend_attachment_states) {
        color_blend_state_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = size_u32(blend_attachment_states.size()),
            .pAttachments = blend_attachment_states.data(),
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
};

}