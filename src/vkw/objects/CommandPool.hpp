#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "CommandBuffers.hpp"

namespace vkw {

namespace objects {

class CommandPool final {
    std::shared_ptr<objects::Device> device_;
    VkCommandPool command_pool_;

public:
    using object_type = VkCommandPool;

    CommandPool() noexcept {}
    CommandPool(std::shared_ptr<objects::Device> device, VkCommandPool&& command_pool) noexcept : device_(device), command_pool_(command_pool) {}
    ~CommandPool() noexcept {
        if(device_) vkDestroyCommandPool(*device_, command_pool_, nullptr);
    }
    CommandPool(const CommandPool& rhs) = delete;
    auto& operator=(const CommandPool& rhs) = delete;
    CommandPool(CommandPool&& rhs) = default;
    CommandPool& operator=(CommandPool&& rhs) = default;

    CommandBuffers allocate_command_buffers(uint32_t buffer_count = 1) {
        VkCommandBufferAllocateInfo allocate_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool_,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = buffer_count,
        };

        std::vector<VkCommandBuffer> cbs(buffer_count);
        vkAllocateCommandBuffers(*device_, &allocate_info, cbs.data());

        return CommandBuffers(device_, command_pool_, std::move(cbs));
    }

};

}

}