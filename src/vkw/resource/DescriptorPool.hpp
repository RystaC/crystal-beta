#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "DescriptorSetLayout.hpp"
#include "DescriptorSets.hpp"

namespace vkw {

namespace resource {

class DescriptorPool final {
    std::shared_ptr<resource::Device> device_;
    VkDescriptorPool pool_;

public:
    using resource_type = VkDescriptorPool;

    DescriptorPool() noexcept {}
    DescriptorPool(std::shared_ptr<resource::Device> device, VkDescriptorPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~DescriptorPool() noexcept {
        if(device_) vkDestroyDescriptorPool(*device_, pool_, nullptr);
    }
    DescriptorPool(const DescriptorPool& rhs) = delete;
    auto& operator=(const DescriptorPool& rhs) = delete;
    DescriptorPool(DescriptorPool&& rhs) = default;
    DescriptorPool& operator=(DescriptorPool&& rhs) = default;

    operator VkDescriptorPool() const noexcept { return pool_; }

    Result<DescriptorSets> allocate_descriptor_sets(const std::vector<std::pair<VkDescriptorSetLayout, VkDescriptorType>>& layouts) {
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
        auto result = vkAllocateDescriptorSets(*device_, &allocate_info, sets.data());

        return Result(DescriptorSets(device_, pool_, std::move(sets), std::move(layouts_t)), result);
    }
};

}

}