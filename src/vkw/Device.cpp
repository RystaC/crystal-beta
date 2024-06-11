#include "Device.hpp"

namespace vkw {

bool Device::init(const std::vector<QueueTarget>& queue_targets, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    if(queue_targets.empty()) return false;

    default_family_index_ = queue_targets[0].family_index;

    std::vector<VkDeviceQueueCreateInfo> queue_infos(queue_targets.size());
    queues_.resize(queue_targets.size());
    for(size_t i = 0; i < queue_targets.size(); ++i) {
        if(queue_targets[i].priorities.empty()) return false;

        queues_[i].resize(queue_targets[i].priorities.size());

        queue_infos[i] = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_targets[i].family_index,
            .queueCount = size_u32(queue_targets[i].priorities.size()),
            .pQueuePriorities = queue_targets[i].priorities.data(),
        };
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
    CHECK_VK_RESULT(vkCreateDevice(physical_device_, &device_info, nullptr, &device), return false;);
    
    device_ = std::make_shared<DeviceEntity>(std::move(device));

    for(size_t i = 0; i < queues_.size(); ++i) {
        for(size_t j = 0; j < queues_[i].size(); ++j) {
            vkGetDeviceQueue(*device_, queue_targets[i].family_index, size_u32(j), &queues_[i][j]);
        }
    }

    return true;
}

}