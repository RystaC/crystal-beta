#pragma once

#include "common.hpp"

namespace vkw {

class Pipeline {
    std::shared_ptr<DeviceEntity> device_;
    VkPipeline pipeline_;
    VkPipelineLayout layout_;

public:
    Pipeline(std::shared_ptr<DeviceEntity> device, VkPipeline&& pipeline, VkPipelineLayout&& layout) noexcept : device_(device), pipeline_(pipeline), layout_(layout) {}
    ~Pipeline() noexcept {
        vkDestroyPipeline(*device_, pipeline_, nullptr);
        vkDestroyPipelineLayout(*device_, layout_, nullptr);
    }
};

}