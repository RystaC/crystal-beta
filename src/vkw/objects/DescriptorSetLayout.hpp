#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class DescriptorPool;

class DescriptorSetLayout {
    std::shared_ptr<objects::Device> device_;
    VkDescriptorSetLayout layout_;
    VkDescriptorType type_;

public:
    using object_type = VkDescriptorSetLayout;

    friend vkw::objects::DescriptorPool;

    DescriptorSetLayout(std::shared_ptr<objects::Device> device, VkDescriptorSetLayout&& layout, VkDescriptorType type) noexcept : device_(device), layout_(layout), type_(type) {}
    ~DescriptorSetLayout() noexcept {
        vkDestroyDescriptorSetLayout(*device_, layout_, nullptr);
    }

    operator VkDescriptorSetLayout() const noexcept { return layout_; }
};

}

}