#pragma once

#include "../common/common.hpp"

#include "../objects/DescriptorSets.hpp"

namespace vkw {

class Device;

namespace descriptor {

class WriteDescriptorSets;

class BufferInfos {
    std::vector<VkDescriptorBufferInfo> infos_;

public:
    friend WriteDescriptorSets;

    auto& add(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
        infos_.emplace_back(
            VkDescriptorBufferInfo {
                .buffer = buffer,
                .offset = offset,
                .range = range,
            }
        );

        return *this;
    }
};

class ImageInfos {
    std::vector<VkDescriptorImageInfo> infos_;

public:
    friend WriteDescriptorSets;

    auto& add(VkImageView image_view, VkImageLayout layout, VkSampler sampler = VK_NULL_HANDLE) {
        infos_.emplace_back(
            VkDescriptorImageInfo {
                .sampler = sampler,
                .imageView = image_view,
                .imageLayout = layout,
            }
        );

        return *this;
    }
};

class WriteDescriptorSets {
    std::vector<VkWriteDescriptorSet> writes_;

public:
    friend vkw::Device;

    auto& add(const std::pair<VkDescriptorSet, VkDescriptorType>& set, uint32_t bind_index, uint32_t array_index, const BufferInfos& buffer_infos) {
        writes_.emplace_back(
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = set.first,
                .dstBinding = bind_index,
                .dstArrayElement = array_index,
                .descriptorCount = size_u32(buffer_infos.infos_.size()),
                .descriptorType = set.second,
                .pBufferInfo = buffer_infos.infos_.data(),
            }
        );

        return *this;
    }

    auto& add(const std::pair<VkDescriptorSet, VkDescriptorType>& set, uint32_t bind_index, uint32_t array_index, const ImageInfos& image_infos) {
        writes_.emplace_back(
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = set.first,
                .dstBinding = bind_index,
                .dstArrayElement = array_index,
                .descriptorCount = size_u32(image_infos.infos_.size()),
                .descriptorType = set.second,
                .pImageInfo = image_infos.infos_.data(),
            }
        );

        return *this;
    }

    auto& add(const std::pair<VkDescriptorSet, VkDescriptorType>& set, uint32_t bind_index, uint32_t array_index, const std::vector<VkBufferView>& buffer_views) {
        writes_.emplace_back(
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = set.first,
                .dstBinding = bind_index,
                .dstArrayElement = array_index,
                .descriptorCount = size_u32(buffer_views.size()),
                .descriptorType = set.second,
                .pTexelBufferView = buffer_views.data(),
            }
        );

        return *this;
    }
};

}

}