#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class PipelineLayout {
    std::shared_ptr<objects::Device> device_;
    VkPipelineLayout layout_;

public:
    PipelineLayout(std::shared_ptr<objects::Device> device, VkPipelineLayout&& layout) noexcept : device_(device), layout_(layout) {}
    ~PipelineLayout() noexcept {
        vkDestroyPipelineLayout(*device_, layout_, nullptr);
    }


    operator VkPipelineLayout() const noexcept { return layout_; }
};

}

}