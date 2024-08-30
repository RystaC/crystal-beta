#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Pipeline {
    std::shared_ptr<objects::Device> device_;
    VkPipeline pipeline_;

public:
    Pipeline(std::shared_ptr<objects::Device> device, VkPipeline&& pipeline) noexcept : device_(device), pipeline_(pipeline) {}
    ~Pipeline() noexcept {
        vkDestroyPipeline(*device_, pipeline_, nullptr);
    }

    operator VkPipeline() const noexcept { return pipeline_; }
};

}

}