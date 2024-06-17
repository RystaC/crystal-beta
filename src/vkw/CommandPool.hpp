#pragma once

#include "common.hpp"

namespace vkw {

class CommandPool {
    std::shared_ptr<CommandPoolEntity> command_pool_;

public:
    CommandPool(std::shared_ptr<CommandPoolEntity> command_pool) noexcept : command_pool_(command_pool) {}
    ~CommandPool() noexcept {
        vkDestroyCommandPool(*command_pool_->device_, *command_pool_, nullptr);
    }

    void allocate_command_buffer(uint32_t buffer_count) {
        VkCommandBufferAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = *command_pool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = buffer_count,
        };

        std::vector<VkCommandBuffer> command_buffers(buffer_count);

        CHECK_VK_RESULT(vkAllocateCommandBuffers(*command_pool_->device_, &allocate_info, command_buffers.data()), return;);
    }

};

}