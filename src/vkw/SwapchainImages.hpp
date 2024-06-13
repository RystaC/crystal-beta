#pragma once

#include "common.hpp"
#include "Swapchain.hpp"

namespace vkw {

class SwapchainImages {
    std::shared_ptr<DeviceEntity> device_;
    std::vector<std::pair<VkImage, VkImageView>> images_;

public:
    SwapchainImages(std::shared_ptr<DeviceEntity> device, std::vector<std::pair<VkImage, VkImageView>> images) noexcept : device_(device), images_(images) {}
    ~SwapchainImages() noexcept {
        for(size_t i = 0; i < images_.size(); ++i) {
            vkDestroyImageView(*device_, images_[i].second, nullptr);
        }
    }
};

}