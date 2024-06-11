#pragma once

#include "common.hpp"

namespace vkw {

class Surface {
    std::shared_ptr<InstanceEntity> instance_;
    VkSurfaceKHR surface_;

public:
    Surface(std::shared_ptr<InstanceEntity> instance, VkSurfaceKHR&& surface) noexcept : instance_(instance), surface_(surface) {}
    ~Surface() noexcept {
        vkDestroySurfaceKHR(*instance_, surface_, nullptr);
    }

    operator VkSurfaceKHR() const noexcept { return surface_; }
};

}