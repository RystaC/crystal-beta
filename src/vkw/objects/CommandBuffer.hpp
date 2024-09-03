#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

#include "../command_buffer/commands.hpp"

namespace vkw {

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

    command_buffer::Commands begin_record() {
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        vkBeginCommandBuffer(command_buffer_, &begin_info);

        return command_buffer::Commands(command_buffer_);
    }
};

}

}