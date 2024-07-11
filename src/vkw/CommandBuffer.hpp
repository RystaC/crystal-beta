#pragma once

#include "common.hpp"

namespace vkw {

class CommandBufferCommands {
    VkCommandBuffer& command_buffer_;

public:
    CommandBufferCommands(VkCommandBuffer& command_buffer) noexcept : command_buffer_(command_buffer) {}

    auto& begin_render_pass(VkFramebuffer framebuffer, VkRenderPass render_pass, VkRect2D render_area, const std::vector<VkClearValue>& clear_values) {
        VkRenderPassBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = render_pass,
            .framebuffer = framebuffer,
            .renderArea = render_area,
            .clearValueCount = size_u32(clear_values.size()),
            .pClearValues = clear_values.data(),
        };
        vkCmdBeginRenderPass(command_buffer_, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
        return *this;
    }

    auto& end_render_pass() {
        vkCmdEndRenderPass(command_buffer_);
        return *this;
    }

    auto& bind_graphics_pipeline(VkPipeline pipeline) {
        vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        return *this;
    }

    auto& bind_vertex_buffer(VkBuffer buffer) {
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(command_buffer_, 0, 1, &buffer, &offset);
        return *this;
    }

    auto& draw(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset = 0u, uint32_t instance_offset = 0u) {
        vkCmdDraw(command_buffer_, vertex_count, instance_count, vertex_offset, instance_offset);
        return *this;
    }

    auto& barrier_image(VkImage& image, VkPipelineStageFlags src_stage, VkPipelineStageFlags dst_stage, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkImageLayout src_layout, VkImageLayout dst_layout) {
        VkImageMemoryBarrier image_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = src_access_mask,
            .dstAccessMask = dst_access_mask,
            .oldLayout = src_layout,
            .newLayout = dst_layout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = VkImageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCmdPipelineBarrier(command_buffer_, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1u, &image_barrier);

        return *this;
    }

};

class CommandBuffer {
    std::shared_ptr<DeviceEntity> device_;
    // std::shared_ptr<CommandPoolEntity> command_pool_;
    VkCommandBuffer command_buffer_;

public:
    // CommandBuffer(std::shared_ptr<CommandPoolEntity> command_pool, VkCommandBuffer&& command_buffer) noexcept : command_pool_(command_pool), command_buffer_(command_buffer) {}
    CommandBuffer(std::shared_ptr<DeviceEntity> device, VkCommandBuffer&& command_buffer) noexcept : device_(device), command_buffer_(command_buffer) {}
    ~CommandBuffer() = default;

    operator VkCommandBuffer() const noexcept { return command_buffer_; }

    template<typename CommandChain>
    auto record_commands(CommandChain command_chain) {
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VkResult result{};
        result = vkBeginCommandBuffer(command_buffer_, &begin_info);
        if(result != VK_SUCCESS) return result;

        CommandBufferCommands commands {command_buffer_};
        command_chain(commands);

        result = vkEndCommandBuffer(command_buffer_);

        return result;
    }
};

}