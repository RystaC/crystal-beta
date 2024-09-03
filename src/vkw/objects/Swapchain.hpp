#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"
#include "Fence.hpp"

namespace vkw {

namespace objects {

class Swapchain {
    std::shared_ptr<objects::Device> device_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;
    uint32_t width_, height_;

public:
    using object_type = VkSwapchainKHR;

    Swapchain(std::shared_ptr<objects::Device> device, VkSwapchainKHR&& swapchain, std::vector<VkImage>&& images, std::vector<VkImageView>&& image_views, uint32_t width, uint32_t height) noexcept :
        device_(device), swapchain_(swapchain), images_(images), image_views_(image_views), width_(width), height_(height)
    {}
    ~Swapchain() noexcept {
        for(auto& image_view : image_views_) {
            vkDestroyImageView(*device_, image_view, nullptr);
        }
        vkDestroySwapchainKHR(*device_, swapchain_, nullptr);
    }

    operator VkSwapchainKHR() const noexcept { return swapchain_; }

    constexpr auto image_view_size() const noexcept { return image_views_.size(); }

    auto& image(size_t index) { return images_[index]; }
    auto& image_view(size_t index) { return image_views_[index]; }
    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }

    uint32_t next_image_index(Fence& fence) {
        VkFence f = fence;
        uint32_t next_index = UINT32_MAX;
        CHECK_VK_RESULT(vkAcquireNextImageKHR(*device_, swapchain_, UINT64_MAX, VK_NULL_HANDLE, f, &next_index), return next_index;);
        fence.wait(UINT64_MAX);
        fence.reset();

        return next_index;
    }
};

}

}