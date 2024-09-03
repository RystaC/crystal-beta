#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

template<VkDescriptorType DescType>
class DescriptorSet {
    std::shared_ptr<object::Device> device_;
    const VkDescriptorPool& descriptor_pool_;
    VkDescriptorSet descriptor_set_;
    static constexpr VkDescriptorType descriptor_type_ = DescType;

public:
    using object_type = VkDescriptorSet;

    DescriptorSet(std::shared_ptr<object::Device> device, const VkDescriptorPool& descriptor_pool, VkDescriptorSet&& descriptor_set) noexcept : device_(device), descriptor_pool_(descriptor_pool), descriptor_set_(descriptor_set) {}

    operator VkDescriptorSet() const noexcept { return descriptor_set_; }

    void update(uint32_t binding, uint32_t array_element, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
        VkDescriptorBufferInfo buffer_info {
            .buffer = buffer,
            .offset = offset,
            .range = range,
        };

        VkWriteDescriptorSet write_descriptor {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set_,
            .dstBinding = binding,
            .dstArrayElement = array_element,
            .descriptorCount = 1u,
            .descriptorType = descriptor_type_,
            .pBufferInfo = &buffer_info,
        };

        vkUpdateDescriptorSets(*device_, 1u, &write_descriptor, 0u, nullptr);
    }

    void update(uint32_t binding, uint32_t array_element, VkSampler sampler, VkImageView image_view, VkImageLayout image_layout) {
        VkDescriptorImageInfo image_info {
            .sampler = sampler,
            .imageView = image_view,
            .imageLayout = image_layout,
        };

        VkWriteDescriptorSet write_descriptor {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set_,
            .dstBinding = binding,
            .dstArrayElement = array_element,
            .descriptorCount = 1u,
            .descriptorType = descriptor_type_,
            .pImageInfo = &image_info,
        };

        vkUpdateDescriptorSets(*device_, 1u, &write_descriptor, 0u, nullptr);
    }

        void update(uint32_t binding, uint32_t array_element, const std::vector<VkDescriptorImageInfo>& image_infos) {

        VkWriteDescriptorSet write_descriptor {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set_,
            .dstBinding = binding,
            .dstArrayElement = array_element,
            .descriptorCount = size_u32(image_infos.size()),
            .descriptorType = descriptor_type_,
            .pImageInfo = image_infos.data(),
        };

        vkUpdateDescriptorSets(*device_, 1u, &write_descriptor, 0u, nullptr);
    }
};

}

}