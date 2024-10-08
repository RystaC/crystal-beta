#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class PipelineLayout final {
    std::shared_ptr<object::Device> device_;
    VkPipelineLayout layout_;

public:
    using object_type = VkPipelineLayout;

    PipelineLayout() noexcept {}
    PipelineLayout(std::shared_ptr<object::Device> device, VkPipelineLayout&& layout) noexcept : device_(device), layout_(layout) {}
    ~PipelineLayout() noexcept {
        if(device_) vkDestroyPipelineLayout(*device_, layout_, nullptr);
    }
    PipelineLayout(const PipelineLayout& rhs) = delete;
    auto& operator=(const PipelineLayout& rhs) = delete;
    PipelineLayout(PipelineLayout&& rhs) = default;
    PipelineLayout& operator=(PipelineLayout&& rhs) = default;

    operator VkPipelineLayout() const noexcept { return layout_; }
};

}

}