#pragma once

#include "common.hpp"
#include "RenderPass.hpp"

namespace vkw {

class Framebuffer {
    std::shared_ptr<DeviceEntity> device_;
    VkFramebuffer framebuffer_;

public:
    Framebuffer(std::shared_ptr<DeviceEntity> device, VkFramebuffer&& framebuffer) noexcept : device_(device), framebuffer_(framebuffer) {}
    ~Framebuffer() noexcept {
        vkDestroyFramebuffer(*device_, framebuffer_, nullptr);
    }

    operator VkFramebuffer() const noexcept { return framebuffer_; }
};

}