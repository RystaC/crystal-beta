#pragma once

#include "common.hpp"
#include "PhysicalDevice.hpp"
#include "CommandPool.hpp"
#include "Swapchain.hpp"
#include "RenderPass.hpp"
#include "Framebuffer.hpp"
#include "ShaderModule.hpp"
#include "GraphicsPipelineStates.hpp"
#include "ComputePipelineStates.hpp"
#include "Pipeline.hpp"

namespace vkw {

struct QueueTarget {
    uint32_t family_index;
    std::vector<float> priorities;
};

class Device {
    std::shared_ptr<DeviceEntity> device_;
    std::shared_ptr<PhysicalDevice> physical_device_;
    VkQueue queue_;
    uint32_t queue_family_index_;

public:
    Device(std::shared_ptr<PhysicalDevice> physical_device) noexcept : device_(nullptr), physical_device_(physical_device), queue_(), queue_family_index_() {}
    ~Device() noexcept {}

    bool init(uint32_t queue_family_index, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::shared_ptr<CommandPool> create_command_pool();

    std::shared_ptr<Swapchain> create_swapchain(const Surface& surface, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, uint32_t width, uint32_t height);

    std::unique_ptr<RenderPass> create_render_pass();

    std::unique_ptr<Framebuffer> create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& image_views, uint32_t width, uint32_t height);

    std::unique_ptr<ShaderModule> create_shader_module(const std::filesystem::path& spirv_path);

    std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineStates& pipeline_states, const RenderPass& render_pass, uint32_t subpass_index);
};

}