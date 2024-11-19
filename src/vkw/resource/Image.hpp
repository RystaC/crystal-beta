#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "ImageView.hpp"

namespace vkw {

namespace resource {

class Image final {
    std::shared_ptr<resource::Device> device_;
    VkImage image_;
    VkDeviceMemory memory_;
    VkFormat format_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkImage;

    Image() noexcept {}
    Image(std::shared_ptr<resource::Device> device, VkImage&& image, VkDeviceMemory&& memory, VkFormat format, const VkAllocationCallbacks* allocator) noexcept : device_(device), image_(image), memory_(memory), format_(format), allocator_(allocator) {}
    ~Image() noexcept {
        if(device_) {
            vkFreeMemory(*device_, memory_, allocator_);
            vkDestroyImage(*device_, image_, allocator_);
        }
    }
    Image(const Image& rhs) = delete;
    auto& operator=(const Image& rhs) = delete;
    Image(Image&& rhs) = default;
    Image& operator=(Image&& rhs) = default;

    operator VkImage() const noexcept { return image_; }

    Result<ImageView> create_image_view(VkImageAspectFlags aspect) {
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
        auto result = vkCreateImageView(*device_, &view_info, allocator_, &image_view);

        return Result(ImageView(device_, std::move(image_view), allocator_), result);
    }
};

}

}