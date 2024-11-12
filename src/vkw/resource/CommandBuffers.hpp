#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

#include "../command_buffer/commands.hpp"

namespace vkw {

namespace queue {

class SubmitInfos;

}

namespace resource {

class CommandBuffers final {
    std::shared_ptr<resource::Device> device_;
    const VkCommandPool& command_pool_;
    std::vector<VkCommandBuffer> command_buffers_;

public:
    friend queue::SubmitInfos;

    CommandBuffers(std::shared_ptr<resource::Device> device, const VkCommandPool& command_pool, std::vector<VkCommandBuffer>&& command_buffers) noexcept : device_(device), command_pool_(command_pool), command_buffers_(command_buffers) {}
    ~CommandBuffers() noexcept {
        if(device_) vkFreeCommandBuffers(*device_, command_pool_, size_u32(command_buffers_.size()), command_buffers_.data());
    }
    CommandBuffers(const CommandBuffers& rhs) = delete;
    auto& operator=(const CommandBuffers& rhs) = delete;
    CommandBuffers(CommandBuffers&& rhs) = default;
    CommandBuffers& operator=(CommandBuffers&& rhs) = default;

    const auto& operator[](size_t i) const & { return command_buffers_[i]; }
    auto& operator[](size_t i) & { return command_buffers_[i]; }
    auto operator[](size_t i) && { return command_buffers_[i]; }

    auto size() const noexcept { return command_buffers_.size(); }

    command_buffer::Commands begin_record(size_t index) {
        VkCommandBufferBeginInfo begin_info {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        vkBeginCommandBuffer(command_buffers_[index], &begin_info);

        return command_buffer::Commands(command_buffers_[index]);
    }

    void reset(size_t index) {
        vkResetCommandBuffer(command_buffers_[0], static_cast<VkCommandBufferResetFlags>(0));
    }
};

}

}