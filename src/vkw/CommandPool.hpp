#pragma once

#include "common.hpp"
#include "debug.hpp"
#include "CommandBuffer.hpp"

namespace vkw {

class CommandPool {
    std::shared_ptr<DeviceEntity> device_;
    VkCommandPool command_pool_;

public:
    CommandPool(std::shared_ptr<DeviceEntity> device, VkCommandPool&& command_pool) noexcept : device_(device), command_pool_(command_pool) {}
    ~CommandPool() noexcept {
        vkDestroyCommandPool(*device_, command_pool_, nullptr);
    }

    std::unique_ptr<CommandBuffer> allocate_command_buffer() {
        VkCommandBufferAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1u,
        };

        VkCommandBuffer command_buffer;
        CHECK_VK_RESULT(vkAllocateCommandBuffers(*device_, &allocate_info, &command_buffer), return {};);

        return std::make_unique<CommandBuffer>(device_, command_pool_, std::move(command_buffer));
    }

};

}