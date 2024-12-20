#pragma once

#include "../common/common.hpp"
#include "Instance.hpp"

namespace vkw {

namespace resource {

class Surface final {
    std::shared_ptr<resource::Instance> instance_;
    VkSurfaceKHR surface_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkSurfaceKHR;

    Surface() noexcept {}
    Surface(std::shared_ptr<resource::Instance> instance, VkSurfaceKHR&& surface, const VkAllocationCallbacks* allocator) noexcept : instance_(instance), surface_(surface), allocator_(allocator) {}
    ~Surface() noexcept {
        if(instance_) vkDestroySurfaceKHR(*instance_, surface_, allocator_);
    }
    Surface(const Surface& rhs) = delete;
    auto& operator=(const Surface& rhs) = delete;
    Surface(Surface&& rhs) = default;
    Surface& operator=(Surface&& rhs) = default;

    operator VkSurfaceKHR() const noexcept { return surface_; }
};

}

}