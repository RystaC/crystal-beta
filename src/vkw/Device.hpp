#pragma once

#include "common.hpp"
#include "debug.hpp"
#include "PhysicalDevice.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "Fence.hpp"
#include "Swapchain.hpp"
#include "RenderPass.hpp"
#include "Framebuffer.hpp"
#include "ShaderModule.hpp"
#include "GraphicsPipelineStates.hpp"
#include "ComputePipelineStates.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"

namespace vkw {

struct QueueTarget {
    uint32_t family_index;
    std::vector<float> priorities;
};

class Device {
    std::shared_ptr<DeviceEntity> device_;
    std::unique_ptr<PhysicalDevice> physical_device_;
    VkQueue queue_;
    uint32_t queue_family_index_;

    VkDeviceMemory allocate_device_memory(const VkMemoryRequirements& requirements);
    template<typename T>
    VkDeviceMemory allocate_memory(const T& storage) {
        VkMemoryRequirements requirements{};
        if constexpr(std::is_same_v<T, VkBuffer>) {
            vkGetBufferMemoryRequirements(*device_, storage, &requirements);
        }
        else if constexpr(std::is_same_v<T, VkImage>) {
            vkGetImageMemoryRequirements(*device_, storage, &requirements);
        }
        else {
            static_assert([]{ return false; }, "[Device::allocate_memory] ERROR: invalid type. expected VkBuffer or VkImage.");
        }

        return allocate_device_memory(requirements);
    }

public:
    Device() noexcept : device_(nullptr), physical_device_(nullptr), queue_(), queue_family_index_() {}
    ~Device() noexcept {}

    bool init(PhysicalDevice&& physical_device, uint32_t queue_family_index, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::unique_ptr<CommandPool> create_command_pool();

    std::unique_ptr<Swapchain> create_swapchain(const Surface& surface, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, uint32_t width, uint32_t height);

    std::unique_ptr<RenderPass> create_render_pass();
    std::unique_ptr<RenderPass> create_render_pass(const RenderPassGraph& render_pass_graph);

    std::unique_ptr<Framebuffer> create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& image_views, uint32_t width, uint32_t height);

    std::unique_ptr<ShaderModule> create_shader_module(const std::filesystem::path& spirv_path);

    std::unique_ptr<Pipeline> create_graphics_pipeline(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges, const GraphicsPipelineStates& pipeline_states, const RenderPass& render_pass, uint32_t subpass_index);

    std::unique_ptr<Fence> create_fence();

    void submit_commands(CommandBuffer& command_buffer, Fence& fence);

    void present(Swapchain& swapchain, uint32_t index);

    template<typename T>
    std::unique_ptr<Buffer<T>> create_buffer_with_data(const std::vector<T>& buffer_data, VkBufferUsageFlags usage) {
        VkBufferCreateInfo buffer_info {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = sizeof(T) * buffer_data.size(),
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VkBuffer buffer{};
        CHECK_VK_RESULT(vkCreateBuffer(*device_, &buffer_info, nullptr, &buffer), return {};);

        auto device_memory = allocate_memory(buffer);

        uint8_t* memory_pointer{};
        CHECK_VK_RESULT(vkMapMemory(*device_, device_memory, 0, sizeof(T) * buffer_data.size(), 0, reinterpret_cast<void**>(&memory_pointer)), return {};);
        memcpy(memory_pointer, buffer_data.data(), sizeof(T) * buffer_data.size());
        vkUnmapMemory(*device_, device_memory);

        CHECK_VK_RESULT(vkBindBufferMemory(*device_, buffer, device_memory, 0), return {};);

        return std::make_unique<Buffer<T>>(device_, std::move(buffer), std::move(device_memory));
    }
};

}