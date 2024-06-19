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

};

class CommandBuffer {
    std::shared_ptr<CommandPoolEntity> command_pool_;
    VkCommandBuffer command_buffer_;

public:
    CommandBuffer(std::shared_ptr<CommandPoolEntity> command_pool, VkCommandBuffer&& command_buffer) noexcept : command_pool_(command_pool), command_buffer_(command_buffer) {}
    ~CommandBuffer() noexcept {
        vkFreeCommandBuffers(*command_pool_->device_, *command_pool_, 1, &command_buffer_);
    }

    template<typename CommandChain>
    auto record_commands(CommandChain command_chain) {
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VkResult result{};
        result = vkBeginCommandBuffer(command_buffer_, &begin_info);
        if(result != VK_SUCCESS) return result;

        command_chain(CommandBufferCommands(command_buffer_));

        result = vkEndCommandBuffer(command_buffer_);

        return result;
    }
};

}