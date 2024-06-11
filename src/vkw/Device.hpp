#pragma once

#include "common.hpp"
#include "PhysicalDevice.hpp"

namespace vkw {

struct QueueTarget {
    uint32_t family_index;
    std::vector<float> priorities;
};

class Device {
    std::shared_ptr<DeviceEntity> device_;
    VkPhysicalDevice physical_device_;
    std::vector<std::vector<VkQueue>> queues_;
    uint32_t default_family_index_;

public:
    Device(VkPhysicalDevice physical_device) noexcept : device_(nullptr), physical_device_(physical_device), queues_(), default_family_index_() {}
    ~Device() noexcept {}

    bool init(const std::vector<QueueTarget>& queue_targets, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
};

}