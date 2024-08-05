#pragma once

#include "common.hpp"

namespace vkw {

class ImageView {
    std::shared_ptr<DeviceEntity> device_;
    VkImageView view_;

public:
    ImageView(std::shared_ptr<DeviceEntity> device, VkImageView&& view) noexcept : device_(device), view_(view) {};
    ~ImageView() noexcept {
        vkDestroyImageView(*device_, view_, nullptr);
    }

    operator VkImageView() const noexcept { return view_; }
};

class Image {
    std::shared_ptr<DeviceEntity> device_;
    VkImage image_;
    VkDeviceMemory memory_;
    VkFormat format_;

public:
    Image(std::shared_ptr<DeviceEntity> device, VkImage&& image, VkDeviceMemory&& memory, VkFormat format) noexcept : device_(device), image_(image), memory_(memory), format_(format) {}
    ~Image() noexcept {
        vkFreeMemory(*device_, memory_, nullptr);
        vkDestroyImage(*device_, image_, nullptr);
    }

    operator VkImage() const noexcept { return image_; }

    std::unique_ptr<ImageView> create_image_view(VkImageAspectFlags aspect) {
        VkImageViewCreateInfo view_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image_,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format_,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = aspect,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VkImageView image_view{};
        CHECK_VK_RESULT(vkCreateImageView(*device_, &view_info, nullptr, &image_view), return {};);

        return std::make_unique<ImageView>(device_, std::move(image_view));
    }
};

}