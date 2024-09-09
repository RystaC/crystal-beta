#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

struct DescriptorSet {
    VkDescriptorSet set;
    VkDescriptorType type;
};

class DescriptorSets {
    std::shared_ptr<objects::Device> device_;
    const VkDescriptorPool& pool_;

    // NOTE: separate descriptor set and descriptor type may be better for freeing descriptor sets
    // (required array of descriptor sets)

    std::vector<DescriptorSet> sets_;

public:
    DescriptorSets(std::shared_ptr<objects::Device> device, const VkDescriptorPool& pool, std::vector<DescriptorSet>&& sets) noexcept : device_(device), pool_(pool), sets_(sets) {}

    const auto& operator[](size_t i) const& { return sets_[i]; }
    auto& operator[](size_t i) & { return sets_[i]; }
    auto operator[](size_t i) && { return sets_[i]; }
};

}

}