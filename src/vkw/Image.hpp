#pragma once

#include "common.hpp"

namespace vkw {

class Image {
    std::shared_ptr<DeviceEntity> device_;
    VkImage image_;
    VkDeviceMemory memory_;
    VkImageView image_view_;

public:
    Image(std::shared_ptr<DeviceEntity> device, VkImage&& image, VkDeviceMemory&& memory, VkImageView&& image_view) noexcept : device_(device), image_(image), memory_(memory), image_view_(image_view) {}
    ~Image() noexcept {
        vkDestroyImageView(*device_, image_view_, nullptr);
        vkFreeMemory(*device_, memory_, nullptr);
        vkDestroyImage(*device_, image_, nullptr);
    }
};

}