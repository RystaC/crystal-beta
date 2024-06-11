#include "Device.hpp"

namespace vkw {

bool Device::init(const VkPhysicalDevice& physical_device, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    uint32_t queue_prop_count{};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_prop_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_props(queue_prop_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_prop_count, queue_props.data());

    uint32_t queue_family_index = std::numeric_limits<uint32_t>::max();
    for(size_t i = 0; i < queue_props.size(); ++i) {
        if((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            queue_family_index = i;
            break;
        }
    }
    if(queue_family_index == std::numeric_limits<uint32_t>::max()) return false;

    float queue_priorities[] = { 0.0f };

    VkDeviceQueueCreateInfo queue_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queue_family_index,
        .queueCount = 1,
        .pQueuePriorities = queue_priorities,
    };

    VkDeviceCreateInfo device_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
        .enabledLayerCount = size_u32(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = size_u32(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkDevice device{};
    CHECK_VK_RESULT(vkCreateDevice(physical_device, &device_info, nullptr, &device), return false;);
    
    device_ = std::make_shared<DeviceEntity>(std::move(device));

    return true;
}

}