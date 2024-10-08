#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class DescriptorPool;

class DescriptorSetLayout final {
    std::shared_ptr<object::Device> device_;
    VkDescriptorSetLayout layout_;
    VkDescriptorType type_;

public:
    using object_type = VkDescriptorSetLayout;

    friend vkw::object::DescriptorPool;

    DescriptorSetLayout() noexcept {}
    DescriptorSetLayout(std::shared_ptr<object::Device> device, VkDescriptorSetLayout&& layout, VkDescriptorType type) noexcept : device_(device), layout_(layout), type_(type) {}
    ~DescriptorSetLayout() noexcept {
        if(device_) vkDestroyDescriptorSetLayout(*device_, layout_, nullptr);
    }
    DescriptorSetLayout(const DescriptorSetLayout& rhs) = delete;
    auto& operator=(const DescriptorSetLayout& rhs) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& rhs) = default;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& rhs) = default;

    operator std::pair<VkDescriptorSetLayout, VkDescriptorType>() const noexcept { return {layout_, type_}; }
    operator VkDescriptorSetLayout() const noexcept { return layout_; }

    auto& layout() const noexcept { return layout_; }
    auto& type() const noexcept { return type_; }
};

}

}