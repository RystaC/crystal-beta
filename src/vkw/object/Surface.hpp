#pragma once

#include "../common/common.hpp"
#include "Instance.hpp"

namespace vkw {

namespace object {

class Surface final {
    std::shared_ptr<object::Instance> instance_;
    VkSurfaceKHR surface_;

public:
    using object_type = VkSurfaceKHR;

    Surface() noexcept {}
    Surface(std::shared_ptr<object::Instance> instance, VkSurfaceKHR&& surface) noexcept : instance_(instance), surface_(surface) {}
    ~Surface() noexcept {
        if(instance_) vkDestroySurfaceKHR(*instance_, surface_, nullptr);
    }
    Surface(const Surface& rhs) = delete;
    auto& operator=(const Surface& rhs) = delete;
    Surface(Surface&& rhs) = default;
    Surface& operator=(Surface&& rhs) = default;

    operator VkSurfaceKHR() const noexcept { return surface_; }
};

}

}