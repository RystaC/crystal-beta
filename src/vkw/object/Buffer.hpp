#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

class Device;

namespace object {

template<typename T>
class Buffer final {
    std::shared_ptr<object::Device> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;

public:
    using value_type = T;
    using object_type = VkBuffer;

    friend vkw::Device;

    Buffer() noexcept {}
    Buffer(std::shared_ptr<object::Device> device, VkBuffer&& buffer, VkDeviceMemory&& memory) noexcept : device_(device), buffer_(buffer), memory_(memory) {}
    ~Buffer() noexcept {
        if(device_) {
            vkFreeMemory(*device_, memory_, nullptr);
            vkDestroyBuffer(*device_, buffer_, nullptr);
        }
    }
    Buffer(const Buffer<T>& rhs) = delete;
    auto& operator=(const Buffer<T>& rhs) = delete;
    Buffer(Buffer<T>&& rhs) = default;
    Buffer<T>& operator=(Buffer<T>&& rhs) = default;

    operator VkBuffer() const noexcept { return buffer_; }
};

}

}