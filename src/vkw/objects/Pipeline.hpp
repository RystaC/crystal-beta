#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Pipeline final {
    std::shared_ptr<objects::Device> device_;
    VkPipeline pipeline_;

public:
    using object_type = VkPipeline;

    Pipeline() noexcept {}
    Pipeline(std::shared_ptr<objects::Device> device, VkPipeline&& pipeline) noexcept : device_(device), pipeline_(pipeline) {}
    ~Pipeline() noexcept {
        if(device_) vkDestroyPipeline(*device_, pipeline_, nullptr);
    }
    Pipeline(const Pipeline& rhs) = delete;
    auto& operator=(const Pipeline& rhs) = delete;
    Pipeline(Pipeline&& rhs) = default;
    Pipeline& operator=(Pipeline&& rhs) = default;

    operator VkPipeline() const noexcept { return pipeline_; }
};

}

}