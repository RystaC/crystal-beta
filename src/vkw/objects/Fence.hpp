#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Fence {
    std::shared_ptr<objects::Device> device_;
    VkFence fence_;

public:
    using object_type = VkFence;

    Fence(std::shared_ptr<objects::Device> device, VkFence&& fence) noexcept : device_(device), fence_(fence) {}
    ~Fence() noexcept {
        vkDestroyFence(*device_, fence_, nullptr);
    }

    operator VkFence() const noexcept { return fence_; }

    auto status() const {
        auto status = vkGetFenceStatus(*device_, fence_);

        return status;
    }

    auto wait(uint64_t timeout) {
        return vkWaitForFences(*device_, 1u, &fence_, VK_TRUE, timeout);
    }

    auto reset() {
        return vkResetFences(*device_, 1u, &fence_);
    }
};

}

}