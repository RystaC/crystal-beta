#pragma once

#include "common.hpp"

namespace vkw {

class RenderPass {
    std::shared_ptr<DeviceEntity> device_;
    VkRenderPass render_pass_;

public:
    RenderPass(std::shared_ptr<DeviceEntity> device, VkRenderPass&& render_pass) noexcept : device_(device), render_pass_(render_pass) {}
    ~RenderPass() noexcept {
        vkDestroyRenderPass(*device_, render_pass_, nullptr);
    }

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}