#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace descriptor {

class WriteDescriptorSets;

}

namespace objects {

class DescriptorSets final {
    std::shared_ptr<objects::Device> device_;
    const VkDescriptorPool& pool_;

    // NOTE: separate descriptor set and descriptor type may be better for freeing descriptor sets
    // (required array of descriptor sets)

    std::vector<VkDescriptorSet> sets_;
    std::vector<VkDescriptorType> types_;

public:
    friend vkw::descriptor::WriteDescriptorSets;
    
    DescriptorSets(std::shared_ptr<objects::Device> device, const VkDescriptorPool& pool, std::vector<VkDescriptorSet>&& sets, std::vector<VkDescriptorType>&& types) noexcept : device_(device), pool_(pool), sets_(sets), types_(types) {}

    const std::pair<VkDescriptorSet, VkDescriptorType> operator[](size_t i) const& { return { sets_[i], types_[i] }; }

    auto size() const noexcept { return sets_.size(); }

    auto& sets() const noexcept { return sets_; }
};

}

}