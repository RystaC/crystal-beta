#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace objects {

class Device {
    VkDevice device_;

public:
    Device(VkDevice&& device) noexcept : device_(device) {}
    ~Device() noexcept {
        vkDestroyDevice(device_, nullptr);
    }

    operator VkDevice() const noexcept { return device_; }
};

}

}