#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Sampler {
    std::shared_ptr<objects::Device> device_;
    VkSampler sampler_;

public:
    Sampler(std::shared_ptr<objects::Device> device, VkSampler&& sampler) noexcept : device_(device), sampler_(sampler) {}
    ~Sampler() noexcept {
        vkDestroySampler(*device_, sampler_, nullptr);
    }

    operator VkSampler() const noexcept { return sampler_; }
};

}

}