#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class RenderPass {
    std::shared_ptr<objects::Device> device_;
    VkRenderPass render_pass_;

public:
    using object_type = VkRenderPass;

    RenderPass() noexcept {}
    RenderPass(std::shared_ptr<objects::Device> device, VkRenderPass&& render_pass) noexcept : device_(device), render_pass_(render_pass) {}
    ~RenderPass() noexcept {
        vkDestroyRenderPass(*device_, render_pass_, nullptr);
    }

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}

}