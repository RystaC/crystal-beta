#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class DeviceMemory final {
    std::shared_ptr<object::Device> device_;
    VkDeviceMemory memory_;

public:
    using object_type = VkDeviceMemory;

    DeviceMemory() noexcept {}
    DeviceMemory(std::shared_ptr<object::Device> device, VkDeviceMemory&& memory) noexcept : device_(device), memory_(memory) {}
    ~DeviceMemory() noexcept {
        if(device_) vkFreeMemory(*device_, memory_, nullptr);
    }
    DeviceMemory(const DeviceMemory& rhs) = delete;
    auto& operator=(const DeviceMemory& rhs) = delete;
    DeviceMemory(DeviceMemory&& rhs) = default;
    DeviceMemory& operator=(DeviceMemory&& rhs) = default;

    operator VkDeviceMemory() const noexcept { return memory_; }
};

}

}