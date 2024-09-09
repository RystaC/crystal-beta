#include "Device.hpp"

namespace vkw {

bool Device::init(const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_infos.infos_.size());
    for(size_t i = 0; i < queue_create_infos.size(); ++i) {
        queue_create_infos[i] = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_infos.infos_[i].family_index,
            .queueCount = size_u32(queue_infos.infos_[i].priorities.size()),
            .pQueuePriorities = queue_infos.infos_[i].priorities.data(),
        };

        queue_counts_[queue_create_infos[i].queueFamilyIndex] += queue_create_infos[i].queueCount;
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

}