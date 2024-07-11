#pragma once

#include "common.hpp"

namespace vkw {

class Fence {
    std::shared_ptr<DeviceEntity> device_;
    VkFence fence_;

public:
    Fence(std::shared_ptr<DeviceEntity> device, VkFence&& fence) noexcept : device_(device), fence_(fence) {}
    ~Fence() noexcept {
        vkDestroyFence(*device_, fence_, nullptr);
    }

    operator VkFence() const noexcept { return fence_; }

    auto wait(uint64_t timeout) {
        return vkWaitForFences(*device_, 1u, &fence_, VK_TRUE, timeout);
    }

    auto reset() {
        return vkResetFences(*device_, 1u, &fence_);
    }
};

}