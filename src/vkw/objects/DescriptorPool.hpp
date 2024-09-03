#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "DescriptorSet.hpp"

namespace vkw {

namespace objects {

class DescriptorPool {
    std::shared_ptr<objects::Device> device_;
    VkDescriptorPool pool_;

public:
    using object_type = VkDescriptorPool;

    DescriptorPool(std::shared_ptr<objects::Device> device, VkDescriptorPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~DescriptorPool() noexcept {
        vkDestroyDescriptorPool(*device_, pool_, nullptr);
    }

    operator VkDescriptorPool() const noexcept { return pool_; }

    template<VkDescriptorType DescType>
    std::unique_ptr<DescriptorSet<DescType>> allocate_descriptor_set(const VkDescriptorSetLayout& layout) {
        VkDescriptorSetAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptor_pool_,
            .descriptorSetCount = 1u,
            .pSetLayouts = &layout,
        };

        VkDescriptorSet descriptor_set{};
        CHECK_VK_RESULT(vkAllocateDescriptorSets(*device_, &allocate_info, &descriptor_set), return {};);

        return std::make_unique<DescriptorSet<DescType>>(device_, descriptor_pool_, std::move(descriptor_set));
    }
};

}

}