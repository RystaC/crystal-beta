#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class BufferView final {
    std::shared_ptr<object::Device> device_;
    VkBufferView buffer_view_;

public:
    using object_type = VkBufferView;

    BufferView() noexcept {}
    BufferView(std::shared_ptr<object::Device> device, VkBufferView&& buffer_view) noexcept : device_(device), buffer_view_(buffer_view) {}
    ~BufferView() noexcept {
        if(device_) vkDestroyBufferView(*device_, buffer_view_, nullptr);
    }
    BufferView(const BufferView& rhs) = delete;
    auto& operator=(const BufferView& rhs) = delete;
    BufferView(BufferView&& rhs) = default;
    BufferView& operator=(BufferView&& rhs) = default;

    operator VkBufferView() const noexcept { return buffer_view_; }
};

}

}