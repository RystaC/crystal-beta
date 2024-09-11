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
        vkDestroyFramebuffer(*device_, framebuffer_, nullptr);
    }
    Framebuffer(const Framebuffer& rhs) = delete;
    auto& operator=(const Framebuffer& rhs) = delete;
    // TODO: implement for other objects like this
    Framebuffer(Framebuffer&& rhs) noexcept {
        *this = std::move(rhs);
    }
    Framebuffer& operator=(Framebuffer&& rhs) noexcept {
        device_ = rhs.device_;
        framebuffer_ = rhs.framebuffer_;
        rhs.framebuffer_ = VK_NULL_HANDLE;
        return *this;
    }

    operator VkFramebuffer() const noexcept { return framebuffer_; }
};

}

}