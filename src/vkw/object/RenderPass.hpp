#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class RenderPass final {
    std::shared_ptr<object::Device> device_;
    VkRenderPass render_pass_;

public:
    using object_type = VkRenderPass;

    RenderPass() noexcept {}
    RenderPass(std::shared_ptr<object::Device> device, VkRenderPass&& render_pass) noexcept : device_(device), render_pass_(render_pass) {}
    ~RenderPass() noexcept {
        if(device_) vkDestroyRenderPass(*device_, render_pass_, nullptr);
    }
    RenderPass(const RenderPass& rhs) = delete;
    auto& operator=(const RenderPass& rhs) = delete;
    RenderPass(RenderPass&& rhs) = default;
    RenderPass& operator=(RenderPass&& rhs) = default;

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}

}