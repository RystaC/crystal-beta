#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class PipelineLayout final {
    std::shared_ptr<resource::Device> device_;
    VkPipelineLayout layout_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkPipelineLayout;

    PipelineLayout() noexcept {}
    PipelineLayout(std::shared_ptr<resource::Device> device, VkPipelineLayout&& layout, const VkAllocationCallbacks* allocator) noexcept : device_(device), layout_(layout), allocator_(allocator) {}
    ~PipelineLayout() noexcept {
        if(device_) vkDestroyPipelineLayout(*device_, layout_, allocator_);
    }
    PipelineLayout(const PipelineLayout& rhs) = delete;
    auto& operator=(const PipelineLayout& rhs) = delete;
    PipelineLayout(PipelineLayout&& rhs) = default;
    PipelineLayout& operator=(PipelineLayout&& rhs) = default;

    operator VkPipelineLayout() const noexcept { return layout_; }
};

}

}