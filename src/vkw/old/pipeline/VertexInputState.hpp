#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class VertexInputState;

class VertexInputBindingDescriptions {
    std::vector<VkVertexInputBindingDescription> descs_;

public:
    friend VertexInputState;

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
    friend VertexInputState;
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

class GraphicsPipelineStates;

class VertexInputState {
    VkPipelineVertexInputStateCreateInfo state_;

public:
    friend GraphicsPipelineStates;
    
    VertexInputState() noexcept : state_{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO} {}
    VertexInputState(const VertexInputBindingDescriptions& bindings, const VertexInputAttributeDescriptions& attributes) :
        state_(
            VkPipelineVertexInputStateCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = size_u32(bindings.descs_.size()),
                .pVertexBindingDescriptions = bindings.descs_.data(),
                .vertexAttributeDescriptionCount = size_u32(attributes.descs_.size()),
                .pVertexAttributeDescriptions = attributes.descs_.data(),
            }
        ) {}
};

}

}