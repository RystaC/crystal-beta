#include "Swapchain.hpp"

namespace vkw {

std::unique_ptr<SwapchainImages> Swapchain::get_images() {
    uint32_t image_count{};
    CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain_, &image_count, nullptr), return {};);
    std::vector<VkImage> images(image_count);
    CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain_, &image_count, images.data()), return {};);

    std::vector<std::pair<VkImage, VkImageView>> swapchain_images(image_count);

    std::vector<VkImageView> image_views(image_count);
    for(size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo view_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format_.format,
            .components = {
                VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        CHECK_VK_RESULT(vkCreateImageView(*device_, &view_info, nullptr, &image_views[i]), return {};);

        swapchain_images[i] = { images[i], image_views[i] };
    }

    return std::make_unique<SwapchainImages>(device_, swapchain_images);
}

}