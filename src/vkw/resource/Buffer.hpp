#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

class Device;

namespace resource {

template<typename T>
class Buffer final {
    std::shared_ptr<resource::Device> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;
    const VkAllocationCallbacks* allocator_;

public:
    using value_type = T;
    using resource_type = VkBuffer;

    friend vkw::Device;

    Buffer() noexcept {}
    Buffer(std::shared_ptr<resource::Device> device, VkBuffer&& buffer, VkDeviceMemory&& memory, const VkAllocationCallbacks* allocator) noexcept : device_(device), buffer_(buffer), memory_(memory), allocator_(allocator) {}
    ~Buffer() noexcept {
        if(device_) {
            vkFreeMemory(*device_, memory_, allocator_);
            vkDestroyBuffer(*device_, buffer_, allocator_);
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