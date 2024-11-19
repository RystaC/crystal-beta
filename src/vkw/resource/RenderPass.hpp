#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class RenderPass final {
    std::shared_ptr<resource::Device> device_;
    VkRenderPass render_pass_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkRenderPass;

    RenderPass() noexcept {}
    RenderPass(std::shared_ptr<resource::Device> device, VkRenderPass&& render_pass, const VkAllocationCallbacks* allocator) noexcept : device_(device), render_pass_(render_pass), allocator_(allocator) {}
    ~RenderPass() noexcept {
        if(device_) vkDestroyRenderPass(*device_, render_pass_, allocator_);
    }
    RenderPass(const RenderPass& rhs) = delete;
    auto& operator=(const RenderPass& rhs) = delete;
    RenderPass(RenderPass&& rhs) = default;
    RenderPass& operator=(RenderPass&& rhs) = default;

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}

}