#pragma once

#include "common.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"

namespace vkw {

class Swapchain {
    std::shared_ptr<DeviceEntity> device_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImageView> image_views_;
    uint32_t width_, height_;

public:
    Swapchain(std::shared_ptr<DeviceEntity> device, VkSwapchainKHR&& swapchain, std::vector<VkImageView>&& image_views, uint32_t width, uint32_t height) noexcept : device_(device), swapchain_(swapchain), image_views_(image_views), width_(width), height_(height) {}
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

    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }
};

}