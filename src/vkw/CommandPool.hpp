#pragma once

#include "common.hpp"
#include "CommandBuffer.hpp"

namespace vkw {

class CommandPool {
    std::shared_ptr<CommandPoolEntity> command_pool_;

public:
    CommandPool(std::shared_ptr<CommandPoolEntity> command_pool) noexcept : command_pool_(command_pool) {}
    ~CommandPool() noexcept {}

    std::unique_ptr<CommandBuffer> allocate_command_buffer() {
        VkCommandBufferAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = *command_pool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer command_buffer{};

        CHECK_VK_RESULT(vkAllocateCommandBuffers(*command_pool_->device_, &allocate_info, &command_buffer), return {};);

        return std::make_unique<CommandBuffer>(command_pool_, std::move(command_buffer));
    }

};

}