#include "Device.hpp"

namespace vkw {

bool Device::init(const std::vector<QueueTarget>& queue_targets, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    if(queue_targets.empty()) return false;

    queues_.resize(queue_targets.size());
    queue_family_indices_.resize(queue_targets.size());

    std::vector<VkDeviceQueueCreateInfo> queue_infos(queue_targets.size());
    for(size_t i = 0; i < queue_targets.size(); ++i) {
        if(queue_targets[i].priorities.empty()) return false;

        queues_[i].resize(queue_targets[i].priorities.size());

        queue_infos[i] = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_targets[i].family_index,
            .queueCount = size_u32(queue_targets[i].priorities.size()),
            .pQueuePriorities = queue_targets[i].priorities.data(),
        };

        queue_family_indices_.push_back(queue_targets[i].family_index);
    }

    VkDeviceCreateInfo device_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = size_u32(queue_infos.size()),
        .pQueueCreateInfos = queue_infos.data(),
        .enabledLayerCount = size_u32(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = size_u32(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkDevice device{};
    CHECK_VK_RESULT(vkCreateDevice(*physical_device_, &device_info, nullptr, &device), return false;);
    
    device_ = std::make_shared<DeviceEntity>(std::move(device));

    for(size_t i = 0; i < queues_.size(); ++i) {
        for(size_t j = 0; j < queues_[i].size(); ++j) {
            vkGetDeviceQueue(*device_, queue_targets[i].family_index, size_u32(j), &queues_[i][j]);
        }
    }

    return true;
}

std::shared_ptr<CommandPool> Device::create_command_pool(uint32_t queue_family_index) {
    VkCommandPoolCreateInfo pool_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queue_family_indices_[queue_family_index],
    };

    VkCommandPool command_pool{};
    CHECK_VK_RESULT(vkCreateCommandPool(*device_, &pool_info, nullptr, &command_pool), return {};);

    return std::make_shared<CommandPool>(device_, std::move(command_pool));
}

std::shared_ptr<Swapchain> Device::create_swapchain(const Surface& surface, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, uint32_t width, uint32_t height, uint32_t queue_family_index) {
    VkBool32 is_supported{};
    vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device_, queue_family_indices_[queue_family_index], surface, &is_supported);
    if(!is_supported) return {};

    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physical_device_, surface, &capabilities);

    uint32_t format_count{};
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_, surface, &format_count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_, surface, &format_count, formats.data());

    uint32_t present_mode_count{};
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_, surface, &present_mode_count, nullptr);
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_, surface, &present_mode_count, present_modes.data());

    VkSwapchainCreateInfoKHR swapchain_info {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2u,
        .imageFormat = desired_format.format,
        .imageColorSpace = desired_format.colorSpace,
        .imageExtent = { width, height },
        .imageArrayLayers = 1u,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = desired_present_mode,
        .clipped = VK_TRUE,
    };

    VkSwapchainKHR swapchain{};
    CHECK_VK_RESULT(vkCreateSwapchainKHR(*device_, &swapchain_info, nullptr, &swapchain), return {};);

    return std::make_shared<Swapchain>(device_, std::move(swapchain), desired_format);
}

}