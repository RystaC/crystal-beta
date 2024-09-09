#pragma once

#include "../common/common.hpp"
#include "../physical_device/SurfaceProperties.hpp"

namespace vkw {

class Device;

namespace objects {

class PhysicalDevice {
    VkPhysicalDevice physical_device_;
    // version, ID, type, name, limits and sparse properties
    VkPhysicalDeviceProperties device_properties_;
    // memory types and memory heaps
    VkPhysicalDeviceMemoryProperties memory_properties_;
    // device specific features
    VkPhysicalDeviceFeatures device_features_;
    // queue types and counts
    std::vector<VkQueueFamilyProperties> queue_family_properties_;

    VkFormatProperties format_properties_(VkFormat format) {
        VkFormatProperties properties{};

        vkGetPhysicalDeviceFormatProperties(physical_device_, format, &properties);

        return properties;
    }

    VkImageFormatProperties image_format_properties_(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) {
        VkImageFormatProperties properties{};

        vkGetPhysicalDeviceImageFormatProperties(physical_device_, format, type, tiling, usage, flags, &properties);

        return properties;
    }

    physical_device::SurfaceProperties surface_capabilities_(VkSurfaceKHR surface, uint32_t queue_family_index) {
        uint32_t format_count{}, present_mode_count{};

        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface, &format_count, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface, &present_mode_count, nullptr);

        physical_device::SurfaceProperties properties(format_count, present_mode_count);

        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, queue_family_index, surface, &properties.supported);
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface, &properties.capabilities);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface, &format_count, properties.formats.data());
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface, &present_mode_count, properties.present_modes.data());

        return properties;
    }

public:
    friend vkw::Device;
    
    using object_type = VkPhysicalDevice;
    
    PhysicalDevice() = default;
    PhysicalDevice(VkPhysicalDevice&& physical_device) noexcept : physical_device_(physical_device) {
        vkGetPhysicalDeviceProperties(physical_device_, &device_properties_);
        vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);
        vkGetPhysicalDeviceFeatures(physical_device_, &device_features_);

        uint32_t queue_prop_count{};
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_prop_count, nullptr);
        queue_family_properties_.resize(queue_prop_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_prop_count, queue_family_properties_.data());
    }
    ~PhysicalDevice() noexcept {}

    operator VkPhysicalDevice() const noexcept { return physical_device_; }

    const char* name() const noexcept { return device_properties_.deviceName; }

    int32_t find_queue_family(VkQueueFlags desired_usage) {
        int32_t index = -1;
        for(size_t i = 0; i < queue_family_properties_.size(); ++i) {
            if((queue_family_properties_[i].queueFlags & desired_usage) != 0) {
                index = static_cast<int32_t>(i);
                break;
            }
        }

        return index;
    }

};

}

}