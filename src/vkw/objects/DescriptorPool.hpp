#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSets.hpp"

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

    DescriptorSets allocate_descriptor_sets(const std::vector<DescriptorSetLayout>& layouts) {
        auto layouts_detached = detach(layouts);

        VkDescriptorSetAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool_,
            .descriptorSetCount = size_u32(layouts_detached.size()),
            .pSetLayouts = layouts_detached.data(),
        };

        std::vector<VkDescriptorSet> sets(layouts.size());
        vkAllocateDescriptorSets(*device_, &allocate_info, sets.data());

        std::vector<DescriptorSet> descriptor_sets(layouts.size());
        for(size_t i = 0; i < descriptor_sets.size(); ++i) {
            descriptor_sets[i] = {sets[i], layouts[i].type_};
        }

        return DescriptorSets(device_, pool_, std::move(descriptor_sets));
    }
};

}

}