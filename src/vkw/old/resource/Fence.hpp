#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class Fence final {
    std::shared_ptr<resource::Device> device_;
    VkFence fence_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkFence;

    Fence() noexcept {}
    Fence(std::shared_ptr<resource::Device> device, VkFence&& fence, const VkAllocationCallbacks* allocator) noexcept : device_(device), fence_(fence), allocator_(allocator) {}
    ~Fence() noexcept {
        if(device_) vkDestroyFence(*device_, fence_, allocator_);
    }
    Fence(const Fence& rhs) = delete;
    auto& operator=(const Fence& rhs) = delete;
    Fence(Fence&& rhs) = default;
    Fence& operator=(Fence&& rhs) = default;

    operator VkFence() const noexcept { return fence_; }

    auto status() const {
        auto status = vkGetFenceStatus(*device_, fence_);

        return status;
    }

    auto wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) {
        return vkWaitForFences(*device_, 1u, &fence_, VK_TRUE, timeout);
    }

    auto reset() {
        return vkResetFences(*device_, 1u, &fence_);
    }
};

}

}