#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class BufferView final {
    std::shared_ptr<resource::Device> device_;
    VkBufferView buffer_view_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkBufferView;

    BufferView() noexcept {}
    BufferView(std::shared_ptr<resource::Device> device, VkBufferView&& buffer_view, const VkAllocationCallbacks* allocator) noexcept : device_(device), buffer_view_(buffer_view), allocator_(allocator) {}
    ~BufferView() noexcept {
        if(device_) vkDestroyBufferView(*device_, buffer_view_, allocator_);
    }
    BufferView(const BufferView& rhs) = delete;
    auto& operator=(const BufferView& rhs) = delete;
    BufferView(BufferView&& rhs) = default;
    BufferView& operator=(BufferView&& rhs) = default;

    operator VkBufferView() const noexcept { return buffer_view_; }
};

}

}