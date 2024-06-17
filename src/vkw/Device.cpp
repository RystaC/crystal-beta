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

    uint32_t image_count{};
    CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, nullptr), return {};);
    std::vector<VkImage> images(image_count);
    CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, images.data()), return {};);

    std::vector<VkImageView> image_views(image_count);
    for(size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo view_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = desired_format.format,
            .components = {
                VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        CHECK_VK_RESULT(vkCreateImageView(*device_, &view_info, nullptr, &image_views[i]), return {};);
    }

    return std::make_shared<Swapchain>(device_, std::move(swapchain), std::move(image_views));
}

std::unique_ptr<RenderPass> Device::create_render_pass() {
    VkAttachmentDescription attachment_desc {
        .format = VK_FORMAT_B8G8R8A8_UNORM,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkAttachmentReference attachment_ref {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_desc {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachment_ref,
    };

    VkRenderPassCreateInfo render_pass_info {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachment_desc,
        .subpassCount = 1,
        .pSubpasses = &subpass_desc,
    };

    VkRenderPass render_pass{};
    CHECK_VK_RESULT(vkCreateRenderPass(*device_, &render_pass_info, nullptr, &render_pass), return {};);

    return std::make_unique<RenderPass>(device_, std::move(render_pass));
}

}