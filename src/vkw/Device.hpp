#pragma once

#include "common/common.hpp"

#include "objects/Device.hpp"
#include "objects/PhysicalDevice.hpp"
#include "objects/Queue.hpp"
#include "objects/Surface.hpp"
#include "objects/Swapchain.hpp"
#include "objects/DeviceMemory.hpp"
#include "objects/Buffer.hpp"
#include "objects/Image.hpp"
#include "objects/RenderPass.hpp"
#include "objects/Framebuffer.hpp"
#include "objects/ShaderModule.hpp"
#include "objects/DescriptorSetLayout.hpp"
#include "objects/DescriptorPool.hpp"
#include "objects/PipelineLayout.hpp"
#include "objects/PipelineCache.hpp"
#include "objects/Pipeline.hpp"

#include "queue/CreateInfos.hpp"

#include "render_pass/AttachmentDescriptions.hpp"
#include "render_pass/AttachmentReferences.hpp"
#include "render_pass/SubpassDescriptions.hpp"
#include "render_pass/SubpassDependencies.hpp"

#include "descriptor/DescriptorSetLayoutBindings.hpp"
#include "descriptor/WriteDescriptorSets.hpp"

#include "pipeline_layout/CreateInfo.hpp"

#include "pipeline/GraphicsPipelineStates.hpp"
#include "pipeline/ComputePipelineStates.hpp"

namespace vkw {

class Device {
    std::shared_ptr<objects::Device> device_;
    objects::PhysicalDevice& physical_device_;

    std::unordered_map<uint32_t, uint32_t> queue_counts_;

    template<typename T>
    VkMemoryRequirements query_memory_requirements_(const T& resource) {
        VkMemoryRequirements requirements{};
        if constexpr(std::is_same_v<T, VkBuffer>) {
            vkGetBufferMemoryRequirements(*device_, resource, &requirements);
        }
        else if constexpr(std::is_same_v<T, VkImage>) {
            vkGetImageMemoryRequirements(*device_, resource, &requirements);
        }
        else {
            static_assert([]{ return false; }, "[vkw::Device::query_memory_requirements_] ERROR: invalid resource type, expected VkBuffer or VkImage.");
        }

        return requirements;
    }

    VkDeviceMemory allocate_memory_with_requirements_(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags desired_properties);

public:
    Device(objects::PhysicalDevice& p_device) noexcept : physical_device_(p_device) {}

    static std::vector<VkLayerProperties> enumerate_layers(objects::PhysicalDevice p_device) {
        uint32_t count{};
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, layers.data());

        return layers;
    }

    static std::vector<VkExtensionProperties> enumerate_extensions(objects::PhysicalDevice p_device, const char* layer_name = nullptr) {
        uint32_t count{};
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, extensions.data());

        return extensions;
    }

    bool init(const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::vector<objects::Queue> create_queues(uint32_t queue_family_index);

    objects::Swapchain create_swapchain(const objects::Surface& surface, uint32_t queue_family_index, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, const VkExtent2D& extent);

    objects::DeviceMemory allocate_memory(VkDeviceSize size, uint32_t memory_type_index);

    template<typename T>
    objects::Buffer<T> create_buffer_with_data(const std::vector<T>& buffer_data, VkBufferUsageFlags usage, VkMemoryPropertyFlags desired_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        VkBufferCreateInfo buffer_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(T) * buffer_data.size(),
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VkBuffer buffer{};
        vkCreateBuffer(*device_, &buffer_info, nullptr, &buffer);

        auto memory_requirements = query_memory_requirements_(buffer);
        auto device_memory = allocate_memory_with_requirements_(memory_requirements, desired_properties);

        uint8_t* memory_pointer{};
        vkMapMemory(*device_, device_memory, 0, sizeof(T) * buffer_data.size(), 0, reinterpret_cast<void**>(&memory_pointer));
        std::memcpy(memory_pointer, buffer_data.data(), sizeof(T) * buffer_data.size());
        vkUnmapMemory(*device_, device_memory);

        vkBindBufferMemory(*device_, buffer, device_memory, 0);

        return objects::Buffer<T>(device_, std::move(buffer), std::move(device_memory));
    }

    objects::Image create_image(const VkExtent2D& extent_2d, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags desired_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    objects::RenderPass create_render_pass(const render_pass::AttachmentDescriptions& attachment_descriptions, const render_pass::SubpassDescriptions& subpass_descriptions, const std::optional<render_pass::SubpassDependencies>& subpass_dependencies = std::nullopt);

    objects::Framebuffer create_framebuffer(const objects::RenderPass& render_pass, const std::vector<VkImageView>& attachments, const VkExtent2D& extent);

    objects::ShaderModule create_shader_module(const std::filesystem::path& spirv_path);

    objects::DescriptorSetLayout create_descriptor_set_layout(const descriptor::DescriptorSetLayoutBindings& layout_bindings);

    objects::DescriptorPool create_descriptor_pool(const std::vector<descriptor::DescriptorSetLayoutBindings>& layouts_for_pool);

    // TODO: deal with desctriptor set copies
    void update_descriptor_sets(const descriptor::WriteDescriptorSets& write_descriptor_sets);

    objects::PipelineLayout create_pipeline_layout(const pipeline_layout::CreateInfo& info);

    objects::PipelineCache create_pipeline_cache();
    objects::PipelineCache create_pipeline_cache(const std::filesystem::path& cache_path);

    // TODO: deal with multiple pipeline creattion
    objects::Pipeline create_pipeline(const pipeline::GraphicsPipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkRenderPass& render_pass, uint32_t subpass_index, const VkPipelineCache& cache = VK_NULL_HANDLE);
    objects::Pipeline create_pipeline(const pipeline::ComputePipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkPipelineCache& cache = VK_NULL_HANDLE);
};

}