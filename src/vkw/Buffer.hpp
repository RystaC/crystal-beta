#pragma once

#include "common.hpp"

namespace vkw {

template<typename T>
class Buffer {
    std::shared_ptr<DeviceEntity> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;

    using _value_type = T;

public:
    Buffer(std::shared_ptr<DeviceEntity> device, VkBuffer&& buffer, VkDeviceMemory&& memory) noexcept : device_(device), buffer_(buffer), memory_(memory) {}
    ~Buffer() noexcept {
        vkFreeMemory(*device_, memory_, nullptr);
        vkDestroyBuffer(*device_, buffer_, nullptr);
    }

    operator VkBuffer() const noexcept { return buffer_; }
};

}