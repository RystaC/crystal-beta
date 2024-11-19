#pragma once

#include "common/common.hpp"

#include "resource/Device.hpp"
#include "resource/PhysicalDevice.hpp"
#include "resource/Queue.hpp"
#include "resource/Surface.hpp"
#include "resource/Swapchain.hpp"
#include "resource/DeviceMemory.hpp"
#include "resource/Buffer.hpp"
#include "resource/Image.hpp"
#include "resource/Sampler.hpp"
#include "resource/RenderPass.hpp"
#include "resource/Framebuffer.hpp"
#include "resource/ShaderModule.hpp"
#include "resource/DescriptorSetLayout.hpp"
#include "resource/DescriptorPool.hpp"
#include "resource/PipelineLayout.hpp"
#include "resource/PipelineCache.hpp"
#include "resource/Pipeline.hpp"
#include "resource/CommandPool.hpp"
#include "resource/Event.hpp"
#include "resource/Fence.hpp"
#include "resource/Semaphore.hpp"

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
    std::shared_ptr<resource::Device> device_;
    const resource::PhysicalDevice& physical_device_;

    std::unordered_map<uint32_t, uint32_t> queue_counts_;

    const VkAllocationCallbacks* allocator_;

    Device(const resource::PhysicalDevice& p_device) noexcept : physical_device_(p_device) {}

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
    static std::vector<VkLayerProperties> enumerate_layers(resource::PhysicalDevice p_device) {
        uint32_t count{};
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, layers.data());

        return layers;
    }

    static std::vector<VkExtensionProperties> enumerate_extensions(resource::PhysicalDevice p_device, const char* layer_name = nullptr) {
        uint32_t count{};
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, extensions.data());

        return extensions;
    }

    static Result<Device> init(const resource::PhysicalDevice& physical_device, const VkPhysicalDeviceFeatures& features, const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers, const VkAllocationCallbacks* allocator);

    std::vector<resource::Queue> create_queues(uint32_t queue_family_index);

    Result<resource::Swapchain> create_swapchain(const resource::Surface& surface, uint32_t queue_family_index, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, const VkExtent2D& extent);

    Result<resource::DeviceMemory> allocate_memory(VkDeviceSize size, uint32_t memory_type_index);

    template<typename T>
    Result<resource::Buffer<T>> create_buffer_with_data(const std::vector<T>& buffer_data, VkBufferUsageFlags usage, VkMemoryPropertyFlags desired_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        VkBufferCreateInfo buffer_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(T) * buffer_data.size(),
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VkBuffer buffer{};
        VkResult result{};
        result = vkCreateBuffer(*device_, &buffer_info, allocator_, &buffer);

        auto memory_requirements = query_memory_requirements_(buffer);
        auto device_memory = allocate_memory_with_requirements_(memory_requirements, desired_properties);

        uint8_t* memory_pointer{};
        result = vkMapMemory(*device_, device_memory, 0, sizeof(T) * buffer_data.size(), 0, reinterpret_cast<void**>(&memory_pointer));
        std::memcpy(memory_pointer, buffer_data.data(), sizeof(T) * buffer_data.size());
        vkUnmapMemory(*device_, device_memory);

        result = vkBindBufferMemory(*device_, buffer, device_memory, 0);

        return Result(resource::Buffer<T>(device_, std::move(buffer), std::move(device_memory), allocator_), result);
    }

    template<typename T>
    void copy_buffer_device_to_host(const resource::Buffer<T>& src_buffer, std::vector<T>& dst_data) {
        uint8_t* pointer{};
        vkMapMemory(*device_, src_buffer.memory_, 0, sizeof(T) * dst_data.size(), 0, reinterpret_cast<void**>(&pointer));
        std::memcpy(dst_data.data(), pointer, sizeof(T) * dst_data.size());
        vkUnmapMemory(*device_, src_buffer.memory_);
    }

    Result<resource::Image> create_image(const VkExtent2D& extent_2d, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags desired_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Result<resource::Sampler> create_sampler(VkFilter min_filter, VkFilter mag_filter, VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v);

    Result<resource::RenderPass> create_render_pass(const render_pass::AttachmentDescriptions& attachment_descriptions, const render_pass::SubpassDescriptions& subpass_descriptions, const std::optional<render_pass::SubpassDependencies>& subpass_dependencies = std::nullopt);

    Result<resource::Framebuffer> create_framebuffer(const resource::RenderPass& render_pass, const std::vector<VkImageView>& attachments, const VkExtent2D& extent);

    Result<resource::ShaderModule> create_shader_module(const std::filesystem::path& spirv_path);

    Result<resource::DescriptorSetLayout> create_descriptor_set_layout(const descriptor::DescriptorSetLayoutBindings& layout_bindings);

    Result<resource::DescriptorPool> create_descriptor_pool(const std::vector<descriptor::DescriptorSetLayoutBindings>& layouts_for_pool);

    // TODO: deal with desctriptor set copies
    void update_descriptor_sets(const descriptor::WriteDescriptorSets& write_descriptor_sets);

    Result<resource::PipelineLayout> create_pipeline_layout(const pipeline_layout::CreateInfo& info);

    Result<resource::PipelineCache> create_pipeline_cache();
    Result<resource::PipelineCache> create_pipeline_cache(const std::filesystem::path& cache_path);

    // TODO: deal with multiple pipeline creattion
    Result<resource::Pipeline> create_pipeline(const pipeline::GraphicsPipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkRenderPass& render_pass, uint32_t subpass_index, const VkPipelineCache& cache = VK_NULL_HANDLE);
    Result<resource::Pipeline> create_pipeline(const pipeline::ComputePipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkPipelineCache& cache = VK_NULL_HANDLE);

    Result<resource::CommandPool> create_command_pool(VkCommandPoolCreateFlags flags, uint32_t queue_family_index);

    Result<resource::Event> create_event();
    Result<resource::Fence> create_fence(bool signaled = false);
    Result<resource::Semaphore> create_semaphore();
};

}