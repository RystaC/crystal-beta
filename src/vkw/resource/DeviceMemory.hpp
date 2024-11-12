#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class DeviceMemory final {
    std::shared_ptr<resource::Device> device_;
    VkDeviceMemory memory_;

public:
    using resource_type = VkDeviceMemory;

    DeviceMemory() noexcept {}
    DeviceMemory(std::shared_ptr<resource::Device> device, VkDeviceMemory&& memory) noexcept : device_(device), memory_(memory) {}
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