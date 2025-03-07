#pragma once

#include "../common/common.hpp"
#include "Device.hpp"
#include "Fence.hpp"
#include "ImageView.hpp"

namespace vkw {

namespace resource {

class Swapchain final {
    std::shared_ptr<resource::Device> device_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImage> images_;
    std::vector<VkImageView> image_views_;
    VkFormat format_;
    VkExtent2D extent_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkSwapchainKHR;

    Swapchain(std::shared_ptr<resource::Device> device, VkSwapchainKHR&& swapchain, std::vector<VkImage>&& images, std::vector<VkImageView>&& image_views, VkFormat format, const VkExtent2D& extent, const VkAllocationCallbacks* allocator) noexcept :
        device_(device), swapchain_(swapchain), images_(images), image_views_(image_views), format_(format), extent_(extent), allocator_(allocator)
    {}
    ~Swapchain() noexcept {
        if(device_) {
            for(size_t i = 0; i < image_views_.size(); ++i) {
                vkDestroyImageView(*device_, image_views_[i], allocator_);
            }
            vkDestroySwapchainKHR(*device_, swapchain_, allocator_);
        }
    }
    Swapchain(const Swapchain& rhs) = delete;
    auto& operator=(const Swapchain& rhs) = delete;
    Swapchain(Swapchain&& rhs) = default;
    Swapchain& operator=(Swapchain&& rhs) = default;

    operator VkSwapchainKHR() const noexcept { return swapchain_; }

    auto size() const noexcept { return images_.size(); }

    const auto& image(size_t i) const { return images_[i]; }
    const auto& image_view(size_t i) const { return image_views_[i]; }

    const auto& extent() const noexcept { return extent_; }

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