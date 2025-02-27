#pragma once

#include "common.hpp"
#include "handle.hpp"

namespace vkw {

class Instance {
    std::shared_ptr<handle::Instance> instance_;
    const VkAllocationCallbacks* allocator_;

    Instance(std::shared_ptr<handle::Instance>&& instance, const VkAllocationCallbacks* allocator) noexcept : instance_(instance), allocator_(allocator) {}

public:
    Instance() noexcept = default;
    ~Instance() noexcept = default;
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance(Instance&&) = default;
    Instance& operator=(Instance&&) = default;

    const auto& operator*() const noexcept { return *instance_; }

    // initialize
    static auto create(const VkInstanceCreateInfo& instance_info, Instance& instance, const VkAllocationCallbacks* allocator = nullptr) {
        VkInstance inst{};
        auto result = vkCreateInstance(&instance_info, allocator, &inst);
        if(result != VK_SUCCESS) {
            return result;
        }

        instance = Instance(std::make_shared<handle::Instance>(std::move(inst), allocator), allocator);

        return result;
    }

    // function pointer
    auto get_proc_addr(const char* name) {
        return vkGetInstanceProcAddr(*instance_, name);
    }

    // physical device
    auto enumerate_physical_devices(std::vector<VkPhysicalDevice>& physical_devices) const {
        uint32_t count{};
        auto result = vkEnumeratePhysicalDevices(*instance_, &count, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        physical_devices.resize(count);
        return vkEnumeratePhysicalDevices(*instance_, &count, physical_devices.data());
    }

    auto enumerate_physical_device_groups(std::vector<VkPhysicalDeviceGroupProperties>& group_properties) {
        uint32_t count{};
        auto result = vkEnumeratePhysicalDeviceGroups(*instance_, &count, nullptr);
        if(result != VK_SUCCESS) {
            return result;
        }
        group_properties.resize(count);
        return vkEnumeratePhysicalDeviceGroups(*instance_, &count, group_properties.data());
    }

    // surface for each platform
#if defined(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
    auto create_surface(const VkWin32SurfaceCreateInfoKHR& surface_info, VkSurfaceKHR& surface) {
        return vkCreateWin32SurfaceKHR(*instance_, &surface_info, allocator_, &surface);
    }
#endif

    // debug utils
#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    auto create_debug_utils_messenger(const VkDebugUtilsMessengerCreateInfoEXT& messenger_info, VkDebugUtilsMessengerEXT& messenger) {
        return vkCreateDebugUtilsMessengerEXT(*instance_, &messenger_info, allocator_, &messenger);
    }
#endif

};

}