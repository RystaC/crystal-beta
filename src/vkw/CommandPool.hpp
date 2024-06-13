#pragma once

#include "common.hpp"

namespace vkw {

class CommandPool {
    std::shared_ptr<DeviceEntity> device_;
    VkCommandPool command_pool_;

public:
    CommandPool(std::shared_ptr<DeviceEntity> device, VkCommandPool&& command_pool) noexcept : device_(device), command_pool_(command_pool) {}
    ~CommandPool() noexcept {
        vkDestroyCommandPool(*device_, command_pool_, nullptr);
    }

    operator VkCommandPool() const noexcept { return command_pool_; }
};

}