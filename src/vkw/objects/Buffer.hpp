#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

struct DrawIndirectBufferData {
    uint32_t group_count_x;
    uint32_t group_count_y;
    uint32_t group_count_z;
};

struct DispatchIndirectBufferData {
    uint32_t vertex_count;
    uint32_t instance_count;
    uint32_t first_vertex;
    uint32_t first_instance;
};

struct DrawIndexedIndirectBufferData {
    uint32_t index_count;
    uint32_t instance_count;
    uint32_t first_index;
    int32_t vertex_offset;
    uint32_t first_instance;
};

namespace objects {

template<typename T>
class Buffer final {
    std::shared_ptr<objects::Device> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;

public:
    using value_type = T;
    using object_type = VkBuffer;

    Buffer() noexcept {}
    Buffer(std::shared_ptr<objects::Device> device, VkBuffer&& buffer, VkDeviceMemory&& memory) noexcept : device_(device), buffer_(buffer), memory_(memory) {}
    ~Buffer() noexcept {
        vkFreeMemory(*device_, memory_, nullptr);
        vkDestroyBuffer(*device_, buffer_, nullptr);
    }
    Buffer(const Buffer<T>& rhs) = delete;
    auto& operator=(const Buffer<T>& rhs) = delete;
    Buffer(Buffer<T>&& rhs) = default;
    Buffer<T>& operator=(Buffer<T>&& rhs) = default;

    operator VkBuffer() const noexcept { return buffer_; }
};

}

}