#pragma once

#include "common.hpp"
#include "Swapchain.hpp"

namespace vkw {

class SwapchainImages {
    std::shared_ptr<DeviceEntity> device_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;

public:
    SwapchainImages(std::shared_ptr<DeviceEntity> device, std::vector<VkImage>&& images, std::vector<VkImageView>&& image_views) noexcept : device_(device), images_(images), image_views_(image_views) {}
    ~SwapchainImages() noexcept {
        for(size_t i = 0; i < images_.size(); ++i) {
            vkDestroyImageView(*device_, image_views_[i], nullptr);
        }
    }
};

}