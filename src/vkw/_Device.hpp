#pragma once

#include "common.hpp"
#include "handle.hpp"

namespace vkw {

class Device {
    std::shared_ptr<handle::Device> device_;
    const VkAllocationCallbacks* allocator_;

    Device(std::shared_ptr<handle::Device>&& device, const VkAllocationCallbacks* allocator) noexcept : device_(device), allocator_(allocator) {}

public:
    Device() noexcept = default;
    ~Device() noexcept = default;
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = default;
    Device& operator=(Device&&) = default;

    const auto& operator*() const noexcept { return *device_; }

    // initialize
    auto create(const VkPhysicalDevice& physical_device, const VkDeviceCreateInfo& device_info, Device& device, const VkAllocationCallbacks* allocator = nullptr) {
        VkDevice dev{};
        auto result = vkCreateDevice(physical_device, &device_info, allocator, &dev);
        if(result != VK_SUCCESS) {
            return result;
        }

        device = Device(std::make_shared<handle::Device>(std::move(dev), allocator), allocator);

        return result;
    }

    // get function pointer
    auto get_proc_addr(const char* name) {
        return vkGetDeviceProcAddr(*device_, name);
    }

    // device queue
    auto get_queue(const VkDeviceQueueInfo2& queue_info, VkQueue& queue) {
        vkGetDeviceQueue2(*device_, &queue_info, &queue);
    }

    // wait idle
    auto wait_idle() {
        return vkDeviceWaitIdle(*device_);
    }

#if defined(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    // swapchain
    auto create_swapchain(const VkSwapchainCreateInfoKHR& swapchain_info, VkSwapchainKHR& swapchain) {
        return vkCreateSwapchainKHR(*device_, &swapchain_info, allocator_, &swapchain);
    }

    auto get_swapchain_images(const VkSwapchainKHR& swapchain, std::vector<VkImage>& images) {
        uint32_t count{};
        auto result = vkGetSwapchainImagesKHR(*device_, swapchain, &count, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        images.resize(count);
        return vkGetSwapchainImagesKHR(*device_, swapchain, &count, images.data());
    }

    auto get_swapchain_status(const VkSwapchainKHR& swapchain) {
        return vkGetSwapchainStatusKHR(*device_, swapchain);
    }
#endif

    // device memory
    auto allocate_memory(const VkMemoryAllocateInfo& allocate_info, VkDeviceMemory& memory) {
        return vkAllocateMemory(*device_, &allocate_info, allocator_, &memory);
    }

    // TODO: use vkMapMemory2KHR (if it is promoted?)
    auto map_memory(const VkDeviceMemory& memory, VkDeviceSize offset, VkDeviceSize size, void** dst_ptr) {
        return vkMapMemory(*device_, memory, offset, size, 0, dst_ptr);
    }

    auto unmap_memory(const VkDeviceMemory& memory) {
        vkUnmapMemory(*device_, memory);
    }

    // image
    auto create_image(const VkImageCreateInfo& image_info, VkImage& image) {
        return vkCreateImage(*device_, &image_info, allocator_, &image);
    }

    auto get_image_memory_requirements(const VkImageMemoryRequirementsInfo2& requirements_info, VkMemoryRequirements2& requirements) {
        vkGetImageMemoryRequirements2(*device_, &requirements_info, &requirements);
    }

    auto bind_image_memory(const std::vector<VkBindImageMemoryInfo>& bind_infos) {
        return vkBindImageMemory2(*device_, size_u32(bind_infos), bind_infos.data());
    }

    auto create_image_view(const VkImageViewCreateInfo& image_view_info, VkImageView& image_view) {
        return vkCreateImageView(*device_, &image_view_info, allocator_, &image_view);
    }

    // buffer
    auto create_buffer(const VkBufferCreateInfo& buffer_info, VkBuffer& buffer) {
        return vkCreateBuffer(*device_, &buffer_info, allocator_, &buffer);
    }

    auto get_buffer_memory_requirements(const VkBufferMemoryRequirementsInfo2& requirements_info, VkMemoryRequirements2& requirements) {
        vkGetBufferMemoryRequirements2(*device_, &requirements_info, &requirements);
    }

    auto bind_buffer_memory(const std::vector<VkBindBufferMemoryInfo>& bind_infos) {
        return vkBindBufferMemory2(*device_, size_u32(bind_infos), bind_infos.data());
    }

    auto create_buffer_view(const VkBufferViewCreateInfo& buffer_view_info, VkBufferView& buffer_view) {
        return vkCreateBufferView(*device_, &buffer_view_info, allocator_, &buffer_view);
    }

    // sampler
    auto create_sampler(const VkSamplerCreateInfo& sampler_info, VkSampler& sampler) {
        return vkCreateSampler(*device_, &sampler_info, allocator_, &sampler);
    }

    auto create_sampler_ycbcr_conversion(const VkSamplerYcbcrConversionCreateInfo& conversion_info, VkSamplerYcbcrConversion& conversion) {
        return vkCreateSamplerYcbcrConversion(*device_, &conversion_info, allocator_, &conversion);
    }

    // render pass
    auto create_render_pass(const VkRenderPassCreateInfo2& render_pass_info, VkRenderPass& render_pass) {
        return vkCreateRenderPass2(*device_, &render_pass_info, allocator_, &render_pass);
    }

    // framebuffer
    auto create_framebuffer(const VkFramebufferCreateInfo& framebuffer_info, VkFramebuffer& framebuffer) {
        return vkCreateFramebuffer(*device_, &framebuffer_info, allocator_, &framebuffer);
    }

    // shader module
    auto create_shader_module(const VkShaderModuleCreateInfo& shader_module_info, VkShaderModule& shader_module) {
        return vkCreateShaderModule(*device_, &shader_module_info, allocator_, &shader_module);
    }

    // descriptor set
    auto create_descriptor_set_layout(const VkDescriptorSetLayoutCreateInfo& layout_info, VkDescriptorSetLayout& layout) {
        return vkCreateDescriptorSetLayout(*device_, &layout_info, allocator_, &layout);
    }

    auto get_descriptor_set_layout_support(const VkDescriptorSetLayoutCreateInfo& layout_info, VkDescriptorSetLayoutSupport& layout_support) {
        vkGetDescriptorSetLayoutSupport(*device_, &layout_info, &layout_support);
    }

    auto create_descriptor_pool(const VkDescriptorPoolCreateInfo& pool_info, VkDescriptorPool& pool) {
        return vkCreateDescriptorPool(*device_, &pool_info, allocator_, &pool);
    }

    auto reset_descriptor_pool(const VkDescriptorPool& pool, VkDescriptorPoolResetFlags flags) {
        return vkResetDescriptorPool(*device_, pool, flags);
    }

    auto allocate_descriptor_sets(const VkDescriptorSetAllocateInfo& allocate_info, std::vector<VkDescriptorSet>& descriptor_sets) {
        return vkAllocateDescriptorSets(*device_, &allocate_info, descriptor_sets.data());
    }

    auto free_descriptor_sets(const VkDescriptorPool& pool, const std::vector<VkDescriptorSet>& sets) {
        return vkFreeDescriptorSets(*device_, pool, size_u32(sets), sets.data());
    }

    auto update_descriptor_sets(const std::vector<VkWriteDescriptorSet>& descriptor_set_writes, const std::vector<VkCopyDescriptorSet>& descriptor_set_copies) {
        return vkUpdateDescriptorSets(*device_, size_u32(descriptor_set_writes), descriptor_set_writes.data(), size_u32(descriptor_set_copies), descriptor_set_copies.data());
    }

    // pipeline
    auto create_pipeline_layout(const VkPipelineLayoutCreateInfo& layout_info, VkPipelineLayout& layout) {
        return vkCreatePipelineLayout(*device_, &layout_info, allocator_, &layout);
    }

    auto create_pipeline_cache(const VkPipelineCacheCreateInfo& cache_info, VkPipelineCache& cache) {
        return vkCreatePipelineCache(*device_, &cache_info, allocator_, &cache);
    }

    auto merge_pipeline_caches(const std::vector<VkPipelineCache>& src_caches, VkPipelineCache& dst_cache) {
        return vkMergePipelineCaches(*device_, dst_cache, size_u32(src_caches), src_caches.data());
    }

    auto get_pipeline_cache_data(const VkPipelineCache& cache, std::vector<uint8_t>& cache_data) {
        size_t cache_size{};
        auto result = vkGetPipelineCacheData(*device_, cache, &cache_size, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        cache_data.resize(cache_size);
        return vkGetPipelineCacheData(*device_, cache, &cache_size, reinterpret_cast<void*>(cache_data.data()));
    }

    auto create_graphics_pipelines(const std::vector<VkGraphicsPipelineCreateInfo>& pipeline_infos, std::vector<VkPipeline>& pipelines) {
        return vkCreateGraphicsPipelines(*device_, VK_NULL_HANDLE, size_u32(pipeline_infos), pipeline_infos.data(), allocator_, pipelines.data());
    }

    auto create_compute_pipelines(const std::vector<VkComputePipelineCreateInfo>& pipeline_infos, std::vector<VkPipeline>& pipelines) {
        return vkCreateComputePipelines(*device_, VK_NULL_HANDLE, size_u32(pipeline_infos), pipeline_infos.data(), allocator_, pipelines.data());
    }

    // quary pool
    auto create_quary_pool(const VkQueryPoolCreateInfo& pool_info, VkQueryPool& pool) {
        return vkCreateQueryPool(*device_, &pool_info, allocator_, &pool);
    }

    auto reset_query_pool(const VkQueryPool& pool, uint32_t first_index, uint32_t query_count) {
        return vkResetQueryPool(*device_, pool, first_index, query_count);
    }

    // command buffer
    auto create_command_pool(const VkCommandPoolCreateInfo& pool_info, VkCommandPool& pool) {
        return vkCreateCommandPool(*device_, &pool_info, allocator_, &pool);
    }

    auto reset_command_pool(const VkCommandPool& pool, VkCommandPoolResetFlags flags) {
        return vkResetCommandPool(*device_, pool, flags);
    }

    auto trim_command_pool(const VkCommandPool& pool, VkCommandPoolTrimFlags flags) {
        return vkTrimCommandPool(*device_, pool, flags);
    }

    auto allocate_command_buffers(const VkCommandBufferAllocateInfo& allocate_info, std::vector<VkCommandBuffer>& command_buffers) {
        return vkAllocateCommandBuffers(*device_, &allocate_info, command_buffers.data());
    }

    auto free_command_buffers(const VkCommandPool& pool, const std::vector<VkCommandBuffer>& buffers) {
        vkFreeCommandBuffers(*device_, pool, size_u32(buffers), buffers.data());
    }

    // sync primitives
    auto create_event(const VkEventCreateInfo& event_info, VkEvent& event) {
        return vkCreateEvent(*device_, &event_info, allocator_, &event);
    }

    auto set_event(const VkEvent& event) {
        return vkSetEvent(*device_, event);
    }

    auto reset_event(const VkEvent& event) {
        return vkResetEvent(*device_, event);
    }

    auto get_event_status(const VkEvent& event) {
        return vkGetEventStatus(*device_, event);
    }

    auto create_fence(const VkFenceCreateInfo& fence_info, VkFence& fence) {
        return vkCreateFence(*device_, &fence_info, allocator_, &fence);
    }

    auto reset_fences(const std::vector<VkFence>& fences) {
        return vkResetFences(*device_, size_u32(fences), fences.data());
    }

    auto wait_for_fences(const std::vector<VkFence>& fences, bool wait_all, uint32_t timeout) {
        return vkWaitForFences(*device_, size_u32(fences), fences.data(), wait_all, timeout);
    }

    auto get_fence_status(const VkFence& fence) {
        return vkGetFenceStatus(*device_, fence);
    }

    auto create_semaphore(const VkSemaphoreCreateInfo& semaphore_info, VkSemaphore& semaphore) {
        return vkCreateSemaphore(*device_, &semaphore_info, allocator_, &semaphore);
    }

    auto signal_semaphore(const VkSemaphoreSignalInfo& signal_info) {
        return vkSignalSemaphore(*device_, &signal_info);
    }

    auto wait_semaphores(const VkSemaphoreWaitInfo& wait_info, uint64_t timeout) {
        return vkWaitSemaphores(*device_, &wait_info, timeout);
    }
};

}