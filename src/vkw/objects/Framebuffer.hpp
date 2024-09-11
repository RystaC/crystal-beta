#pragma once

#include "../common/common.hpp"
#include "Device.hpp"


namespace vkw {

namespace objects {

class Framebuffer final {
    std::shared_ptr<objects::Device> device_;
    VkFramebuffer framebuffer_;

public:
    using object_type = VkFramebuffer;

    Framebuffer() noexcept {}
    Framebuffer(std::shared_ptr<objects::Device> device, VkFramebuffer&& framebuffer) noexcept : device_(device), framebuffer_(framebuffer) {}
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