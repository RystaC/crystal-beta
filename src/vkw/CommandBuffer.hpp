#pragma once

#include "common.hpp"

namespace vkw {

class CommandBuffer {
    std::shared_ptr<CommandPoolEntity> command_pool_;
    VkCommandBuffer command_buffer_;

public:
    CommandBuffer(std::shared_ptr<CommandPoolEntity> command_pool, VkCommandBuffer&& command_buffer) noexcept : command_pool_(command_pool), command_buffer_(command_buffer) {}
    ~CommandBuffer() noexcept {
        vkFreeCommandBuffers(*command_pool_->device_, *command_pool_, 1, &command_buffer_);
    }
};

}