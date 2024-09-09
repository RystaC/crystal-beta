#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace descriptor {

class DescriptorSetLayoutBindings {
    std::vector<VkDescriptorSetLayoutBinding> bindings_;
    // limit only one descriptor type for each layout
    VkDescriptorType type_;

public:
    friend vkw::Device;

    DescriptorSetLayoutBindings(VkDescriptorType type) noexcept : type_(type) {}

    auto& add(uint32_t binding, uint32_t count, VkShaderStageFlags stage_flags, const std::optional<std::vector<VkSampler>> immutable_samplers = std::nullopt) {
        bindings_.emplace_back(
            VkDescriptorSetLayoutBinding {
                .binding = binding,
                .descriptorType = type_,
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