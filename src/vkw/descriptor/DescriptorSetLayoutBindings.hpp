#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace descriptor {

class DescriptorSetLayoutBindings {
    std::vector<VkDescriptorSetLayoutBinding> bindings_;

public:
    friend vkw::Device;

    auto& add(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage_flags, const std::optional<std::vector<VkSampler>> immutable_samplers = std::nullopt) {
        bindings_.emplace_back(
            VkDescriptorSetLayoutBinding {
                .binding = binding,
                .descriptorType = type,
                .descriptorCount = count,
                .stageFlags = stage_flags,
                .pImmutableSamplers = immutable_samplers.has_value() ? immutable_samplers.value().data() : nullptr,
            }
        );
        
        return *this;
    }
};

}

}