#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "Fence.hpp"
#include "ImageView.hpp"

namespace vkw {

namespace objects {

class Swapchain {
    std::shared_ptr<objects::Device> device_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImage> images_;
    VkFormat format_;
    uint32_t width_, height_;

public:
    using object_type = VkSwapchainKHR;

    Swapchain(std::shared_ptr<objects::Device> device, VkSwapchainKHR&& swapchain, std::vector<VkImage>&& images, VkFormat format, uint32_t width, uint32_t height) noexcept :
        device_(device), swapchain_(swapchain), images_(images), format_(format), width_(width), height_(height)
    {}
    ~Swapchain() noexcept {
        vkDestroySwapchainKHR(*device_, swapchain_, nullptr);
    }

    operator VkSwapchainKHR() const noexcept { return swapchain_; }

    const VkImage& operator[](size_t i) const& { return images_[i]; }
    VkImage& operator[](size_t i) & { return images_[i]; }
    VkImage operator[](size_t i) && { return images_[i]; }

    auto width() const noexcept { return width_; }
    auto height() const noexcept { return height_; }

    std::vector<ImageView> create_image_views() {
        std::vector<ImageView> image_views(images_.size());

        for(size_t i = 0; i < image_views.size(); ++i) {
            VkImageViewCreateInfo view_info {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = images_[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = format_,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            };

            VkImageView image_view{};

            CHECK_VK_RESULT(vkCreateImageView(*device_, &view_info, nullptr, &image_view), return {};);

            image_views[i] = ImageView(device_, std::move(image_view));
        }

        return image_views;
    }

    uint32_t next_image_index(Fence& fence) {
        uint32_t next_index = UINT32_MAX;
        CHECK_VK_RESULT(vkAcquireNextImageKHR(*device_, swapchain_, UINT64_MAX, VK_NULL_HANDLE, fence, &next_index), return next_index;);
        fence.wait(UINT64_MAX);
        fence.reset();

        return next_index;
    }
};

}

}