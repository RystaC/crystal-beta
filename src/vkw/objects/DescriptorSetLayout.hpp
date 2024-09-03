#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class DescriptorSetLayout {
    std::shared_ptr<objects::Device> device_;
    VkDescriptorSetLayout layout_;

public:
    using object_type = VkDescriptorSetLayout;

    DescriptorSetLayout(std::shared_ptr<objects::Device> device, VkDescriptorSetLayout&& layout) noexcept : device_(device), layout_(layout) {}
    ~DescriptorSetLayout() noexcept {
        vkDestroyDescriptorSetLayout(*device_, layout_, nullptr);
    }

    operator VkDescriptorSetLayout() const noexcept { return layout_; }
};

}

}