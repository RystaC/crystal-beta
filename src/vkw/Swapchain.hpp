#pragma once

#include "common.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"

namespace vkw {

class Swapchain {
    std::shared_ptr<DeviceEntity> device_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImageView> image_views_;

public:
    Swapchain(std::shared_ptr<DeviceEntity> device, VkSwapchainKHR&& swapchain, std::vector<VkImageView>&& image_views) noexcept : device_(device), swapchain_(swapchain), image_views_(image_views) {}
    ~Swapchain() noexcept {
        for(auto& image_view : image_views_) {
            vkDestroyImageView(*device_, image_view, nullptr);
        }
        vkDestroySwapchainKHR(*device_, swapchain_, nullptr);
    }

    operator VkSwapchainKHR() const noexcept { return swapchain_; }

    constexpr auto size() const noexcept { return image_views_.size(); }

    const auto& operator[](size_t i) const& { return image_views_[i]; }
    // auto& operator[](size_t i) & { return image_views_[i]; }
    // auto operator[](size_t i) && { return image_views_[i]; }
};

}