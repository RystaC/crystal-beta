#pragma once

#include "../common/common.hpp"

#include "ShaderStage.hpp"
#include "VertexInputState.hpp"
#include "InputAssemblyState.hpp"
#include "TessellationState.hpp"
#include "ViewportState.hpp"
#include "RasterizationState.hpp"
#include "MultisampleState.hpp"
#include "DepthStencilState.hpp"
#include "ColorBlendState.hpp"
#include "DynamicState.hpp"

namespace vkw {

class Device;

namespace pipeline {

class GraphicsPipelineStates {
    VkGraphicsPipelineCreateInfo pipeline_info_;

public:
    friend Device;

    GraphicsPipelineStates() noexcept : pipeline_info_{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO} {}

    auto& shader_stages(const GraphicsShaderStages& stages) {
        pipeline_info_.stageCount = size_u32(stages.shader_stages_.size());
        pipeline_info_.pStages = stages.shader_stages_.data();

        return *this;
    }

    auto& vertex_input(const VertexInputState& state) {
        pipeline_info_.pVertexInputState = &state.state_;

        return *this;
    }

    auto& input_assembly(const InputAssemblyState& state) {
        pipeline_info_.pInputAssemblyState = &state.state_;

        return *this;
    }

    auto& tessellation(const TessellationState& state) {
        pipeline_info_.pTessellationState = &state.state_;

        return *this;
    }

    auto& viewport(const ViewportState& state) {
        pipeline_info_.pViewportState = &state.state_;

        return *this;
    }

    auto& rasterization(const RasterizarionState& state) {
        pipeline_info_.pRasterizationState = &state.state_;

        return *this;
    }

    auto& multisample(const MultisampleState& state) {
        pipeline_info_.pMultisampleState = &state.state_;

        return *this;
    }

    auto& depth_stencil(const DepthStencilState& state) {
        pipeline_info_.pDepthStencilState = &state.state_;

        return *this;
    }

    auto& color_blend(const ColorBlendState& state) {
        pipeline_info_.pColorBlendState = &state.state_;

        return *this;
    }

    auto& dynamic(const DynamicState& state) {
        pipeline_info_.pDynamicState = &state.state_;

        return *this;
    }
};

}

}