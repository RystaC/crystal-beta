#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class ColorBlendState;

class ColorBlendAttachmentStates {
    std::vector<VkPipelineColorBlendAttachmentState> states_;

public:
    friend ColorBlendState;

    auto& add(VkBool32 blend_enable = VK_FALSE, 
        VkBlendFactor src_color = VK_BLEND_FACTOR_ONE, VkBlendFactor dst_color = VK_BLEND_FACTOR_ZERO, VkBlendOp color_op = VK_BLEND_OP_ADD,
        VkBlendFactor src_alpha = VK_BLEND_FACTOR_ONE, VkBlendFactor dst_alpha = VK_BLEND_FACTOR_ZERO, VkBlendOp alpha_op = VK_BLEND_OP_ADD,
        VkColorComponentFlags write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    ) {
        states_.emplace_back(
            VkPipelineColorBlendAttachmentState {
                .blendEnable = blend_enable,
                .srcColorBlendFactor = src_color,
                .dstColorBlendFactor = dst_color,
                .colorBlendOp = color_op,
                .srcAlphaBlendFactor = src_alpha,
                .dstAlphaBlendFactor = dst_alpha,
                .alphaBlendOp = alpha_op,
                .colorWriteMask = write_mask,
            }
        );

        return *this;
    }
};

class GraphicsPipelineStates;

class ColorBlendState {
    VkPipelineColorBlendStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    ColorBlendState(const ColorBlendAttachmentStates& attachments, VkBool32 logic_op_enable = VK_FALSE, VkLogicOp logic_op = VK_LOGIC_OP_NO_OP, const glm::vec4& blend_constants = {0.0f, 0.0f, 0.0f, 0.0f}) :
        state_(
            VkPipelineColorBlendStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = logic_op_enable,
                .logicOp = logic_op,
                .attachmentCount = size_u32(attachments.states_.size()),
                .pAttachments = attachments.states_.data(),
                .blendConstants = { blend_constants.r, blend_constants.g, blend_constants.b, blend_constants.a },
            }
        ) {}
};

}

}