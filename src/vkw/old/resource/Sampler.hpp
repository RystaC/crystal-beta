#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class Sampler final {
    std::shared_ptr<resource::Device> device_;
    VkSampler sampler_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkSampler;

    Sampler() noexcept {}
    Sampler(std::shared_ptr<resource::Device> device, VkSampler&& sampler, const VkAllocationCallbacks* allocator) noexcept : device_(device), sampler_(sampler), allocator_(allocator) {}
    ~Sampler() noexcept {
        if(device_) vkDestroySampler(*device_, sampler_, allocator_);
    }
    Sampler(const Sampler& rhs) = delete;
    auto& operator=(const Sampler& rhs) = delete;
    Sampler(Sampler&& rhs) = default;
    Sampler& operator=(Sampler&& rhs) = default;

    operator VkSampler() const noexcept { return sampler_; }
};

}

}