#pragma once

#include "../common/common.hpp"
#include "Device.hpp"


namespace vkw {

namespace resource {

class Framebuffer final {
    std::shared_ptr<resource::Device> device_;
    VkFramebuffer framebuffer_;

public:
    using resource_type = VkFramebuffer;

    Framebuffer() noexcept {}
    Framebuffer(std::shared_ptr<resource::Device> device, VkFramebuffer&& framebuffer) noexcept : device_(device), framebuffer_(framebuffer) {}
    ~Framebuffer() noexcept {
        if(device_) vkDestroyFramebuffer(*device_, framebuffer_, nullptr);
    }
    Framebuffer(const Framebuffer& rhs) = delete;
    auto& operator=(const Framebuffer& rhs) = delete;
    Framebuffer(Framebuffer&& rhs) noexcept = default;
    Framebuffer& operator=(Framebuffer&& rhs) noexcept = default;

    operator VkFramebuffer() const noexcept { return framebuffer_; }
};

}

}