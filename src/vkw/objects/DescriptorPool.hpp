#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSets.hpp"

namespace vkw {

namespace objects {

class DescriptorPool final {
    std::shared_ptr<objects::Device> device_;
    VkDescriptorPool pool_;

public:
    using object_type = VkDescriptorPool;

    DescriptorPool() noexcept {}
    DescriptorPool(std::shared_ptr<objects::Device> device, VkDescriptorPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~DescriptorPool() noexcept {
        if(device_) vkDestroyDescriptorPool(*device_, pool_, nullptr);
    }
    DescriptorPool(const DescriptorPool& rhs) = delete;
    auto& operator=(const DescriptorPool& rhs) = delete;
    DescriptorPool(DescriptorPool&& rhs) = default;
    DescriptorPool& operator=(DescriptorPool&& rhs) = default;

    operator VkDescriptorPool() const noexcept { return pool_; }

    DescriptorSets allocate_descriptor_sets(const std::vector<std::pair<VkDescriptorSetLayout, VkDescriptorType>>& layouts) {
        // TODO: fix fumbled algorithm
        std::vector<VkDescriptorSetLayout> layouts_l(layouts.size());
        std::vector<VkDescriptorType> layouts_t(layouts.size());
        // transform vector: vector(layout, type) -> vector(layout), vector(type)
        std::transform(layouts.begin(), layouts.end(), layouts_l.begin(), [](const auto& l) { return l.first; });
        std::transform(layouts.begin(), layouts.end(), layouts_t.begin(), [](const auto& l) { return l.second; });

        VkDescriptorSetAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool_,
            .descriptorSetCount = size_u32(layouts_l.size()),
            .pSetLayouts = layouts_l.data(),
        };

        std::vector<VkDescriptorSet> sets(layouts.size());
        vkAllocateDescriptorSets(*device_, &allocate_info, sets.data());

        std::vector<DescriptorSet> descriptor_sets(layouts.size());
        for(size_t i = 0; i < descriptor_sets.size(); ++i) {
            descriptor_sets[i] = {sets[i], layouts_t[i]};
        }

        return DescriptorSets(device_, pool_, std::move(descriptor_sets));
    }
};

}

}