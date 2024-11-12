#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace resource {

class Device final {
    VkDevice device_;

public:
    using resource_type = VkDevice;

    Device(VkDevice&& device) noexcept : device_(device) {}
    ~Device() noexcept {
        vkDestroyDevice(device_, nullptr);
    }
    Device(const Device& rhs) = delete;
    auto& operator=(const Device& rhs) = delete;
    Device(Device&& rhs) = default;
    Device& operator=(Device&& rhs) = default;

    operator VkDevice() const noexcept { return device_; }
};

}

}