#pragma once

#include "common.hpp"
#include "Surface.hpp"
#include "PhysicalDevice.hpp"
#include "SwapchainImages.hpp"

namespace vkw {

class Swapchain {
    std::shared_ptr<DeviceEntity> device_;
    VkSwapchainKHR swapchain_;
    VkSurfaceFormatKHR format_;

public:
    Swapchain(std::shared_ptr<DeviceEntity> device, VkSwapchainKHR&& swapchain, const VkSurfaceFormatKHR& format) noexcept : device_(device), swapchain_(swapchain), format_(format) {}
    ~Swapchain() noexcept {
        vkDestroySwapchainKHR(*device_, swapchain_, nullptr);
    }

    std::unique_ptr<SwapchainImages> get_images();
};

}