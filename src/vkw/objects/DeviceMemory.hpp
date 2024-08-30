#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class DeviceMemory {
    std::shared_ptr<objects::Device> device_;
    VkDeviceMemory memory_;

public:
    DeviceMemory(std::shared_ptr<objects::Device> device, VkDeviceMemory&& memory) noexcept : device_(device), memory_(memory) {}
    ~DeviceMemory() noexcept {
        vkFreeMemory(*device_, memory_, nullptr);
    }

    operator VkDeviceMemory() const noexcept { return memory_; }
};

}

}