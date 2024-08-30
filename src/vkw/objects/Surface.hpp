#pragma once

#include "../common/common.hpp"
#include "Instance.hpp"

namespace vkw {

namespace objects {

class Surface {
    std::shared_ptr<objects::Instance> instance_;
    VkSurfaceKHR surface_;

public:
    Surface(std::shared_ptr<objects::Instance> instance, VkSurfaceKHR&& surface) noexcept : instance_(instance), surface_(surface) {}
    ~Surface() noexcept {
        vkDestroySurfaceKHR(*instance_, surface_, nullptr);
    }

    operator VkSurfaceKHR() const noexcept { return surface_; }
};

}

}