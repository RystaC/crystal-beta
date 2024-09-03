#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

class CommandBufferCommands {
    const VkCommandBuffer& command_buffer_;

public:
    CommandBufferCommands(const VkCommandBuffer& command_buffer) noexcept : command_buffer_(command_buffer) {}

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

    auto& next_subpass() {
        vkCmdNextSubpass(command_buffer_, VK_SUBPASS_CONTENTS_INLINE);
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

    auto& bind_vertex_buffer(uint32_t binding, VkBuffer buffer) {
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(command_buffer_, binding, 1, &buffer, &offset);
        return *this;
    }

    auto& bind_index_buffer(VkBuffer buffer, VkIndexType index_type) {
        vkCmdBindIndexBuffer(command_buffer_, buffer, 0, index_type);
        return *this;
    }

    auto& push_constants(VkPipelineLayout layout, VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data) {
        vkCmdPushConstants(command_buffer_, layout, stage, offset, size, data);
        return *this;
    }

    auto& bind_descriptor_sets(VkPipelineBindPoint bind_point, VkPipelineLayout layout, const std::vector<VkDescriptorSet>& descriptor_sets) {
        vkCmdBindDescriptorSets(command_buffer_, bind_point, layout, 0u, size_u32(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
        return *this;
    }

    auto& draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0u, uint32_t first_instance = 0u) {
        vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
        return *this;
    }

    auto& draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index = 0u, int32_t vertex_offset = 0, uint32_t first_instance = 0u) {
        vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
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

namespace objects {

class CommandBuffer {
    std::shared_ptr<objects::Device> device_;
    const VkCommandPool& command_pool_;
    VkCommandBuffer command_buffer_;

public:
    using object_type = VkCommandBuffer;

    CommandBuffer(std::shared_ptr<objects::Device> device, const VkCommandPool& command_pool, VkCommandBuffer&& command_buffer) noexcept : device_(device), command_pool_(command_pool), command_buffer_(command_buffer) {}
    ~CommandBuffer() noexcept {
        vkFreeCommandBuffers(*device_, command_pool_, 1, &command_buffer_);
    }

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

}