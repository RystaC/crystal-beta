#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class Pipeline final {
    std::shared_ptr<resource::Device> device_;
    VkPipeline pipeline_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkPipeline;

    Pipeline() noexcept {}
    Pipeline(std::shared_ptr<resource::Device> device, VkPipeline&& pipeline, const VkAllocationCallbacks* allocator) noexcept : device_(device), pipeline_(pipeline), allocator_(allocator) {}
    ~Pipeline() noexcept {
        if(device_) vkDestroyPipeline(*device_, pipeline_, allocator_);
    }
    Pipeline(const Pipeline& rhs) = delete;
    auto& operator=(const Pipeline& rhs) = delete;
    Pipeline(Pipeline&& rhs) = default;
    Pipeline& operator=(Pipeline&& rhs) = default;

    operator VkPipeline() const noexcept { return pipeline_; }
};

}

}