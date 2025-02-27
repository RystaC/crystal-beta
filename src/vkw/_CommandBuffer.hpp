#pragma once

#include "common.hpp"

namespace vkw {

class CommandBuffer {
    const VkCommandBuffer& command_buffer_;

public:
    CommandBuffer(const VkCommandBuffer& command_buffer) noexcept : command_buffer_(command_buffer) {}
    ~CommandBuffer() noexcept = default;

    operator VkCommandBuffer() const noexcept { return command_buffer_; }

    auto begin(const VkCommandBufferBeginInfo& begin_info) {
        return vkBeginCommandBuffer(command_buffer_, &begin_info);
    }

    auto end() {
        return vkEndCommandBuffer(command_buffer_);
    }

    auto reset(VkCommandBufferResetFlags flags) {
        return vkResetCommandBuffer(command_buffer_, flags);
    }

    // query
    auto begin_query(const VkQueryPool& query_pool, uint32_t query_index, VkQueryControlFlags query_flags) {
        vkCmdBeginQuery(command_buffer_, query_pool, query_index, query_flags);
    }

    auto end_query(const VkQueryPool& query_pool, uint32_t query_index) {
        vkCmdEndQuery(command_buffer_, query_pool, query_index);
    }

    // render pass
    auto begin_render_pass(const VkRenderPassBeginInfo& begin_info, const VkSubpassBeginInfo& subpass_info) {
        vkCmdBeginRenderPass2(command_buffer_, &begin_info, &subpass_info);
    }

    auto next_subpass(const VkSubpassBeginInfo& next_subpass_info, const VkSubpassEndInfo& current_subpass_info) {
        vkCmdNextSubpass2(command_buffer_, &next_subpass_info, &current_subpass_info);
    }

    auto end_render_pass(const VkSubpassEndInfo& end_info) {
        vkCmdEndRenderPass2(command_buffer_, &end_info);
    }

    // bind APIs

    // descriptor set
    auto bind_descriptor_sets(VkPipelineBindPoint bind_point, const VkPipelineLayout& pipeline_layout, uint32_t first_index, const std::vector<VkDescriptorSet>& descriptor_sets, const std::vector<uint32_t>& dynamic_offsets = {}) {
        vkCmdBindDescriptorSets(command_buffer_, bind_point, pipeline_layout, first_index, size_u32(descriptor_sets), descriptor_sets.data(), size_u32(dynamic_offsets), dynamic_offsets.data());
    }

    // index buffer
    auto bind_index_buffer(const VkBuffer index_buffer, VkDeviceSize offset, VkIndexType index_type) {
        vkCmdBindIndexBuffer(command_buffer_, index_buffer, offset, index_type);
    }

    // pipeline
    auto bind_pipeline(VkPipelineBindPoint bind_point, const VkPipeline& pipeline) {
        vkCmdBindPipeline(command_buffer_, bind_point, pipeline);
    }

    // vertex buffer
    auto bind_vertex_buffers(uint32_t first_index, const std::vector<VkBuffer>& buffers, const std::vector<VkDeviceSize>& offsets) {
        vkCmdBindVertexBuffers(command_buffer_, first_index, size_u32(buffers), buffers.data(), offsets.data());
    }

    // end bind APIs

    // blit image
    auto blit_image(const VkBlitImageInfo2& blit_info) {
        vkCmdBlitImage2(command_buffer_, &blit_info);
    }

    // clear APIs

    // attachment
    auto clear_attachments(const std::vector<VkClearAttachment>& attachments, const std::vector<VkClearRect>& rects) {
        vkCmdClearAttachments(command_buffer_, size_u32(attachments), attachments.data(), size_u32(rects), rects.data());
    }

    // color image
    auto clear_color_image(const VkImage image, const VkImageLayout& image_layout, const VkClearColorValue& color, const std::vector<VkImageSubresourceRange>& ranges) {
        vkCmdClearColorImage(command_buffer_, image, image_layout, &color, size_u32(ranges), ranges.data());
    }

    // depth stencil image
    auto clear_depth_stencil_image(const VkImage& image, const VkImageLayout& image_layout, const VkClearDepthStencilValue& depth_stencil, const std::vector<VkImageSubresourceRange>& ranges) {
        vkCmdClearDepthStencilImage(command_buffer_, image, image_layout, &depth_stencil, size_u32(ranges), ranges.data());
    }

    // claer APIs end

    // copy APIs

    // buffer to buffer
    auto copy_buffer_to_buffer(const VkBuffer& src_buffer, const VkBuffer& dst_buffer, const std::vector<VkBufferCopy>& regions) {
        vkCmdCopyBuffer(command_buffer_, src_buffer, dst_buffer, size_u32(regions), regions.data());
    }

    // buffer to image
    auto copy_buffer_to_image(const VkBuffer& src_buffer, const VkImage& dst_image, const VkImageLayout& dst_image_layout, const std::vector<VkBufferImageCopy>& regions) {
        vkCmdCopyBufferToImage(command_buffer_, src_buffer, dst_image, dst_image_layout, size_u32(regions), regions.data());
    }

    // image to image
    auto copy_image_to_image(const VkImage& src_image, const VkImageLayout& src_image_layout, const VkImage& dst_image, const VkImageLayout& dst_image_layout, const std::vector<VkImageCopy>& regions) {
        vkCmdCopyImage(command_buffer_, src_image, src_image_layout, dst_image, dst_image_layout, size_u32(regions), regions.data());
    }

    // image to buffer
    auto copy_image_to_buffer(const VkImage& src_image, const VkImageLayout& src_image_layout, const VkBuffer& dst_buffer, const std::vector<VkBufferImageCopy>& regions) {
        vkCmdCopyImageToBuffer(command_buffer_, src_image, src_image_layout, dst_buffer, size_u32(regions), regions.data());
    }

    // query pool result
    auto copy_query_pool_results(const VkQueryPool& query_pool, uint32_t first_index, uint32_t query_count, const VkBuffer& dst_buffer, VkDeviceSize offset, VkDeviceSize stride, VkQueryResultFlags flags) {
        vkCmdCopyQueryPoolResults(command_buffer_, query_pool, first_index, query_count, dst_buffer, offset, stride, flags);
    }

    // end copy APIs

    // dispatch and draw APIs

    // dispatch
    auto dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) {
        vkCmdDispatch(command_buffer_, group_count_x, group_count_y, group_count_z);
    }

    // dispatch base
    auto dispatch_base(uint32_t base_group_x, uint32_t base_group_y, uint32_t base_group_z, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) {
        vkCmdDispatchBase(command_buffer_, base_group_x, base_group_y, base_group_z, group_count_x, group_count_y, group_count_z);
    }

    // dispatch indirect
    auto dispatch_indirect(const VkBuffer& buffer, VkDeviceSize offset) {
        vkCmdDispatchIndirect(command_buffer_, buffer, offset);
    }

    // draw
    auto draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
        vkCmdDraw(command_buffer_, vertex_count, instance_count, first_vertex, first_instance);
    }

    // draw indirect
    auto draw_indirect(const VkBuffer& buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride) {
        vkCmdDrawIndirect(command_buffer_, buffer, offset, draw_count, stride);
    }

    // draw indirect count
    auto draw_indirect_count(const VkBuffer& buffer, VkDeviceSize offset, const VkBuffer& count_buffer, VkDeviceSize count_offset, uint32_t max_count, uint32_t stride) {
        vkCmdDrawIndirectCount(command_buffer_, buffer, offset, count_buffer, count_offset, max_count, stride);
    }

    // draw indexed
    auto draw_indexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) {
        vkCmdDrawIndexed(command_buffer_, index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    // draw indexed indirect
    auto draw_indexed_indirect(const VkBuffer& buffer, VkDeviceSize offset, uint32_t draw_count, uint32_t stride) {
        vkCmdDrawIndexedIndirect(command_buffer_, buffer, offset, draw_count, stride);
    }

    // draw indexed indirect count
    auto draw_indexed_indirect_count(const VkBuffer& buffer, VkDeviceSize offset, const VkBuffer& count_buffer, VkDeviceSize count_offset, uint32_t max_count, uint32_t stride) {
        vkCmdDrawIndexedIndirectCount(command_buffer_, buffer, offset, count_buffer, count_offset, max_count, stride);
    }

    // end dispatch and draw APIs

    // fill buffer
    auto fill_buffer(const VkBuffer& dst_buffer, VkDeviceSize dst_offset, VkDeviceSize size, uint32_t data) {
        vkCmdFillBuffer(command_buffer_, dst_buffer, dst_offset, size, data);
    }

    // update buffer
    auto update_buffer(const VkBuffer& dst_buffer, VkDeviceSize dst_offset, VkDeviceSize size, const void* data) {
        vkCmdUpdateBuffer(command_buffer_, dst_buffer, dst_offset, size, data);
    }

    // barrier
    auto pipeline_barrier(const VkDependencyInfo& dependency_info) {
        vkCmdPipelineBarrier2(command_buffer_, &dependency_info);
    }

    // push constants
    auto push_constants(const VkPipelineLayout& pipeline_layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, const void* values) {
        vkCmdPushConstants(command_buffer_, pipeline_layout, stage_flags, offset, size, values);
    }

    // reset query pool
    auto reset_query_pool(const VkQueryPool& query_pool, uint32_t first_index, uint32_t query_count) {
        vkCmdResetQueryPool(command_buffer_, query_pool, first_index, query_count);
    }

    auto write_timestamp(VkPipelineStageFlags2 stage, const VkQueryPool& query_pool, uint32_t query_index) {
        vkCmdWriteTimestamp2(command_buffer_, stage, query_pool, query_index);
    }

    // resolve image
    auto resolve_image(const VkResolveImageInfo2& resolve_info) {
        vkCmdResolveImage2(command_buffer_, &resolve_info);
    }

    // event
    auto set_event(const VkEvent& event, const VkDependencyInfo& dependency_info) {
        vkCmdSetEvent2(command_buffer_, event, &dependency_info);
    }

    auto reset_event(const VkEvent& event, VkPipelineStageFlags2 stage_mask) {
        vkCmdResetEvent2(command_buffer_, event, stage_mask);
    }

    auto wait_events(const std::vector<VkEvent>& events, const std::vector<VkDependencyInfo>& dependency_infos) {
        vkCmdWaitEvents2(command_buffer_, size_u32(events), events.data(), dependency_infos.data());
    }
};

}