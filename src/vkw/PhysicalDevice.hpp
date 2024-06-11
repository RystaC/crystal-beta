#pragma once

#include "common.hpp"

namespace vkw {

class PhysicalDevice {
    VkPhysicalDevice physical_device_;
    VkPhysicalDeviceProperties device_properties_;
    VkPhysicalDeviceMemoryProperties memory_properties_;
    std::vector<VkQueueFamilyProperties> queue_family_properties_;

public:
    PhysicalDevice(VkPhysicalDevice&& physical_device) noexcept : physical_device_(physical_device), device_properties_(), memory_properties_(), queue_family_properties_() {
        vkGetPhysicalDeviceProperties(physical_device_, &device_properties_);
        vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);

        uint32_t queue_prop_count{};
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_prop_count, nullptr);
        queue_family_properties_.resize(queue_prop_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_prop_count, queue_family_properties_.data());
    }
    ~PhysicalDevice() noexcept {}

    operator VkPhysicalDevice() const noexcept { return physical_device_; }

    int32_t find_queue_family(VkQueueFlags desired_usage, size_t start_index = 0) {
        int32_t index = -1;
        for(size_t i = start_index; i < queue_family_properties_.size(); ++i) {
            if((queue_family_properties_[i].queueFlags & desired_usage) != 0) {
                index = static_cast<int32_t>(i);
                break;
            }
        }

        return index;
    }

};

}