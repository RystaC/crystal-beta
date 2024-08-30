#pragma once

#include "../common/common.hpp"
#include "Device.hpp"


namespace vkw {

namespace objects {

class Framebuffer {
    std::shared_ptr<objects::Device> device_;
    VkFramebuffer framebuffer_;

public:
    Framebuffer(std::shared_ptr<objects::Device> device, VkFramebuffer&& framebuffer) noexcept : device_(device), framebuffer_(framebuffer) {}
    ~Framebuffer() noexcept {
        vkDestroyFramebuffer(*device_, framebuffer_, nullptr);
    }

    operator VkFramebuffer() const noexcept { return framebuffer_; }
};

}

}