#include "Device.hpp"

namespace vkw {

bool Device::init(const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_infos.infos_.size());
    auto j = queue_infos.infos_.begin();
    for(size_t i = 0; i < queue_create_infos.size(); ++i) {
        queue_create_infos[i] = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = j->first,
            .queueCount = size_u32(j->second.size()),
            .pQueuePriorities = j->second.data(),
        };

        queue_counts_[queue_create_infos[i].queueFamilyIndex] += queue_create_infos[i].queueCount;
        ++j;
    }

    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = size_u32(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = size_u32(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = size_u32(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkDevice device{};
    CHECK_VK_RESULT(vkCreateDevice(physical_device_, &device_info, nullptr, &device), return false;);

    device_ = std::make_shared<objects::Device>(std::move(device));

    return true;
}

std::vector<objects::Queue> Device::create_queues(uint32_t queue_family_index) {
    std::vector<VkQueue> raw_queues(queue_counts_[queue_family_index]);
    for(uint32_t i = 0; i < queue_counts_[queue_family_index]; ++i) {
        vkGetDeviceQueue(*device_, queue_family_index, i, &raw_queues[i]);
    }

    std::vector<objects::Queue> queues(queue_counts_[queue_family_index]);
    std::transform(raw_queues.begin(), raw_queues.end(), queues.begin(), [&](auto& q) { return objects::Queue(std::move(q), queue_family_index); });

    return queues;
}

objects::Swapchain Device::create_swapchain(const objects::Surface& surface, uint32_t queue_family_index, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, const VkExtent2D& extent) {
    auto surface_capabilities = physical_device_.surface_capabilities_(surface, queue_family_index);

    // TODO: check surface compatibilities for swapchain

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2,
        .imageFormat = desired_format.format,
        .imageColorSpace = desired_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = desired_present_mode,
        .clipped = VK_TRUE,
    };

    VkSwapchainKHR swapchain{};
    vkCreateSwapchainKHR(*device_, &swapchain_info, nullptr, &swapchain);

    uint32_t image_count{};
    vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, nullptr);
    std::vector<VkImage> images(image_count);
    vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, images.data());

    std::vector<VkImageView> image_views(image_count);
    for(size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo view_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = desired_format.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        vkCreateImageView(*device_, &view_info, nullptr, &image_views[i]);
    }    

    return objects::Swapchain(device_, std::move(swapchain), std::move(images), std::move(image_views), desired_format.format, extent);
}

objects::DeviceMemory Device::allocate_memory(VkDeviceSize size, uint32_t memory_type_index) {
    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = size,
        .memoryTypeIndex = memory_type_index,
    };

    VkDeviceMemory memory{};
    vkAllocateMemory(*device_, &allocate_info, nullptr, &memory);

    return objects::DeviceMemory(device_, std::move(memory));
}

VkDeviceMemory Device::allocate_memory_with_requirements_(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags desired_properties) {
    uint32_t memory_type_index = std::numeric_limits<uint32_t>::max();
    auto& memory_properties = physical_device_.memory_properties_;
    uint32_t type_bits = 1;
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if((requirements.memoryTypeBits & type_bits) != 0) {
            if((memory_properties.memoryTypes[i].propertyFlags & desired_properties) != 0) {
                memory_type_index = i;
                break;
            }
        }
    }
    if(memory_type_index == std::numeric_limits<uint32_t>::max()) {
        std::cerr << "[vkw::Device::allocate_memory_with_requirements_] ERROR: cannot find memory type with resource requirements and desired properties." << std::endl;
        return VK_NULL_HANDLE;
    }

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memory_type_index,
    };

    VkDeviceMemory memory{};
    vkAllocateMemory(*device_, &allocate_info, nullptr, &memory);

    return memory;
}

objects::Image Device::create_image(const VkExtent2D& extent_2d, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags desired_properties) {
    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = extent_2d.width,
            .height = extent_2d.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImage image{};
    vkCreateImage(*device_, &image_info, nullptr, &image);

    auto requirements = query_memory_requirements_(image);
    auto device_memory = allocate_memory_with_requirements_(requirements, desired_properties);

    vkBindImageMemory(*device_, image, device_memory, 0);

    return objects::Image(device_, std::move(image), std::move(device_memory), format);
}


}