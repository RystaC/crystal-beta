#pragma once

#include "common.hpp"

namespace vkw {

class PhysicalDevice {
    const VkPhysicalDevice& physical_device_;

public:
    PhysicalDevice(const VkPhysicalDevice& physical_device) noexcept : physical_device_(physical_device) {}
    ~PhysicalDevice() noexcept = default;

    operator VkPhysicalDevice() const noexcept { return physical_device_; }

    auto get_properties(VkPhysicalDeviceProperties2& propeties) {
        vkGetPhysicalDeviceProperties2(physical_device_, &propeties);
    }

    auto get_features(VkPhysicalDeviceFeatures2& features) {
        vkGetPhysicalDeviceFeatures2(physical_device_, &features);
    }

    auto get_memory_properties(VkPhysicalDeviceMemoryProperties2& memory_properties) {
        vkGetPhysicalDeviceMemoryProperties2(physical_device_, &memory_properties);
    }

    auto get_format_properties(const VkFormat& format, VkFormatProperties2& format_properties) {
        vkGetPhysicalDeviceFormatProperties2(physical_device_, format, &format_properties);
    }

    auto get_image_format_properties(const VkPhysicalDeviceImageFormatInfo2& format_info, VkImageFormatProperties2& format_properties) {
        vkGetPhysicalDeviceImageFormatProperties2(physical_device_, &format_info, &format_properties);
    }

    auto get_sparse_image_format_properties(const VkPhysicalDeviceSparseImageFormatInfo2& format_info, std::vector<VkSparseImageFormatProperties2> format_properties) {
        uint32_t count{};
        vkGetPhysicalDeviceSparseImageFormatProperties2(physical_device_, &format_info, &count, nullptr);
        format_properties.resize(count);
        vkGetPhysicalDeviceSparseImageFormatProperties2(physical_device_, &format_info, &count, format_properties.data());
    }

    auto get_queue_family_properties(std::vector<VkQueueFamilyProperties2>& queue_family_properties) {
        uint32_t count{};
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device_, &count, nullptr);
        queue_family_properties.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties2(physical_device_, &count, queue_family_properties.data());
    }

#if defined(VK_KHR_SURFACE_EXTENSION_NAME)
    auto get_surface_capabilities(const VkPhysicalDeviceSurfaceInfo2KHR& surface_info, VkSurfaceCapabilities2KHR& capabilities) {
        return vkGetPhysicalDeviceSurfaceCapabilities2KHR(physical_device_, &surface_info, &capabilities);
    }

    auto get_surface_formats(const VkPhysicalDeviceSurfaceInfo2KHR& surface_info, std::vector<VkSurfaceFormat2KHR>& formats) {
        uint32_t count{};
        auto result = vkGetPhysicalDeviceSurfaceFormats2KHR(physical_device_, &surface_info, &count, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        formats.resize(count);
        return vkGetPhysicalDeviceSurfaceFormats2KHR(physical_device_, &surface_info, &count, formats.data());
    }

    auto get_surface_present_modes(const VkSurfaceKHR& surface, std::vector<VkPresentModeKHR>& modes) {
        uint32_t count{};
        auto result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface, &count, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        modes.resize(count);
        return vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_, surface, &count, modes.data());
    }

    auto get_surface_support(uint32_t queue_family_index, const VkSurfaceKHR& surface, VkBool32& supported) {
        return vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, queue_family_index, surface, &supported);
    }
#endif
};

}