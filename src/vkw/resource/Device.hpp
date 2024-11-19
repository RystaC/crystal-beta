#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace resource {

class Device final {
    VkDevice device_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkDevice;

    Device(VkDevice&& device, const VkAllocationCallbacks* allocator) noexcept : device_(device), allocator_(allocator) {}
    ~Device() noexcept {
        vkDestroyDevice(device_, allocator_);
    }
    Device(const Device& rhs) = delete;
    auto& operator=(const Device& rhs) = delete;
    Device(Device&& rhs) = default;
    Device& operator=(Device&& rhs) = default;

    operator VkDevice() const noexcept { return device_; }
};

}

}