#pragma once

#include "common.hpp"

namespace vkw {

class Device;

class DescriptorSetLayoutBindings {
    std::vector<VkDescriptorSetLayoutBinding> bindings_;

public:
    friend Device;

    auto& add(uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage_flags, const std::optional<std::vector<VkSampler>> immutable_samplers = std::nullopt) {
        bindings_.emplace_back(
            VkDescriptorSetLayoutBinding {
                .binding = binding,
                .descriptorType = type,
                .descriptorCount = count,
                .stageFlags = stage_flags,
                .pImmutableSamplers = immutable_samplers.has_value() ? immutable_samplers.value().data() : nullptr,
            }
        );
        
        return *this;
    }
};

class DescriptorSetLayout {
    std::shared_ptr<DeviceEntity> device_;
    VkDescriptorSetLayout descriptor_layout_;

public:
    DescriptorSetLayout(std::shared_ptr<DeviceEntity> device, VkDescriptorSetLayout&& descriptor_layout) noexcept : device_(device), descriptor_layout_(descriptor_layout) {}
    ~DescriptorSetLayout() noexcept {
        vkDestroyDescriptorSetLayout(*device_, descriptor_layout_, nullptr);
    }

    operator VkDescriptorSetLayout() const noexcept { return descriptor_layout_; }
};

template<VkDescriptorType DescType>
class DescriptorSet;

class DescriptorPool {
    std::shared_ptr<DeviceEntity> device_;
    VkDescriptorPool descriptor_pool_;

public:
    DescriptorPool(std::shared_ptr<DeviceEntity> device, VkDescriptorPool&& descriptor_pool) noexcept : device_(device), descriptor_pool_(descriptor_pool) {}
    ~DescriptorPool() noexcept {
        vkDestroyDescriptorPool(*device_, descriptor_pool_, nullptr);
    }

    operator VkDescriptorPool() const noexcept { return descriptor_pool_; }

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

template<VkDescriptorType DescType>
class DescriptorSet {
    std::shared_ptr<DeviceEntity> device_;
    const VkDescriptorPool& descriptor_pool_;
    VkDescriptorSet descriptor_set_;
    static constexpr VkDescriptorType descriptor_type_ = DescType;

public:
    DescriptorSet(std::shared_ptr<DeviceEntity> device, const VkDescriptorPool& descriptor_pool, VkDescriptorSet&& descriptor_set) noexcept : device_(device), descriptor_pool_(descriptor_pool), descriptor_set_(descriptor_set) {}

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