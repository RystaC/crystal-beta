#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class Sampler final {
    std::shared_ptr<object::Device> device_;
    VkSampler sampler_;

public:
    using object_type = VkSampler;

    Sampler() noexcept {}
    Sampler(std::shared_ptr<object::Device> device, VkSampler&& sampler) noexcept : device_(device), sampler_(sampler) {}
    ~Sampler() noexcept {
        if(device_) vkDestroySampler(*device_, sampler_, nullptr);
    }
    Sampler(const Sampler& rhs) = delete;
    auto& operator=(const Sampler& rhs) = delete;
    Sampler(Sampler&& rhs) = default;
    Sampler& operator=(Sampler&& rhs) = default;

    operator VkSampler() const noexcept { return sampler_; }
};

}

}