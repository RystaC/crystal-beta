#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace command_buffer {

class Commands {
    const VkCommandBuffer& command_buffer_;

public:
    Commands(const VkCommandBuffer& command_buffer) noexcept : command_buffer_(command_buffer) {}

    // render pass commands
    [[nodiscard]]
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

    [[nodiscard]]
    auto& next_subpass() {
        vkCmdNextSubpass(command_buffer_, VK_SUBPASS_CONTENTS_INLINE);
        return *this;
    }

    [[nodiscard]]
    auto& end_render_pass() {
        vkCmdEndRenderPass(command_buffer_);
        return *this;
    }

    // binding commands
    [[nodiscard]]
    auto& bind_pipline(VkPipelineBindPoint bind_point, VkPipeline pipeline) {
        vkCmdBindPipeline(command_buffer_, bind_point, pipeline);
        return *this;
    }

    [[nodiscard]]
    auto& bind_vertex_buffers(uint32_t first_binding, const std::vector<VkBuffer>& buffers) {
        std::vector<VkDeviceSize> offsets(buffers.size(), 0);
        vkCmdBindVertexBuffers(command_buffer_, first_binding, size_u32(buffers.size()), buffers.data(), offsets.data());
        return *this;
    }

    [[nodiscard]]
    auto& bind_index_buffer(VkBuffer buffer, VkIndexType index_type) {
        vkCmdBindIndexBuffer(command_buffer_, buffer, 0, index_type);
        return *this;
    }

    [[nodiscard]]
    auto& bind_descriptor_sets(VkPipelineBindPoint bind_point, VkPipelineLayout layout, uint32_t first_set, const std::vector<VkDescriptorSet>& descriptor_sets) {
        vkCmdBindDescriptorSets(command_buffer_, bind_point, layout, first_set, size_u32(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
        return *this;
    }

    // push constant command
    [[nodiscard]]
    auto& push_constants(VkPipelineLayout layout, VkShaderStageFlags stage, uint32_t offset, uint32_t size, const void* data) {
        vkCmdPushConstants(command_buffer_, layout, stage, offset, size, data);
        return *this;
    }

    // draw commands
    [[nodiscard]]
    auto& draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0u, uint32_t first_instance = 0u) {
        vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
        return *this;
    }

    [[nodiscard]]
    auto& draw_indirect(VkBuffer buffer, VkDeviceSize offset, uint32_t count, uint32_t stride) {
        vkCmdDrawIndirect(command_buffer_, buffer, offset, count, stride);
        return *this;
    }

    [[nodiscard]]
    auto& draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index = 0u, int32_t vertex_offset = 0, uint32_t first_instance = 0u) {
        vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
        return *this;
    }

    [[nodiscard]]
    auto& draw_indexed_indirect(VkBuffer buffer, VkDeviceSize offset, uint32_t count, uint32_t stride) {
        vkCmdDrawIndexedIndirect(command_buffer_, buffer, offset, count, stride);
        return *this;
    }

    // dispatch commands
    [[nodiscard]]
    auto& dispatch(uint32_t group_x, uint32_t group_y, uint32_t group_z) {
        vkCmdDispatch(command_buffer_, group_x, group_y, group_z);
        return *this;
    }

    [[nodiscard]]
    auto& dispatch_indirect(VkBuffer buffer, VkDeviceSize offset) {
        vkCmdDispatchIndirect(command_buffer_, buffer, offset);
        return *this;
    }

    // sync commands
    [[nodiscard]]
    auto& barrier_image(VkImage& image, Transition<VkPipelineStageFlags> stage_transition, Transition<VkAccessFlags> access_transition, Transition<VkImageLayout> layout_transition) {
        VkImageMemoryBarrier image_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = access_transition.src,
            .dstAccessMask = access_transition.dst,
            .oldLayout = layout_transition.src,
            .newLayout = layout_transition.dst,
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

        vkCmdPipelineBarrier(command_buffer_, stage_transition.src, stage_transition.dst, 0, 0, nullptr, 0, nullptr, 1u, &image_barrier);

        return *this;
    }

    // copy commands

    // query commands
    [[nodiscard]]
    auto& begin_query(VkQueryPool pool, uint32_t index, VkQueryControlFlags flags = 0) {
        vkCmdBeginQuery(command_buffer_, pool, index, flags);
        return *this;
    }

    [[nodiscard]]
    auto& end_query(VkQueryPool pool, uint32_t index) {
        vkCmdEndQuery(command_buffer_, pool, index);
        return *this;
    }

    // dynamic state commands

    // end record (must call)
    void end_record() {
        vkEndCommandBuffer(command_buffer_);
    }

};

}

}