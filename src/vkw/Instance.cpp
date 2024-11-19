#include "Instance.hpp"

namespace vkw {

Result<Instance> Instance::init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers, const VkAllocationCallbacks* allocator) {
    auto instance = Instance();

    VkApplicationInfo application_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "crystal-beta demo",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .pEngineName = "crystal-beta",
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo instance_info {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = size_u32(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = size_u32(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkInstance instance_entity{};
    auto result = vkCreateInstance(&instance_info, allocator, &instance_entity);

    instance.instance_ = std::make_shared<resource::Instance>(std::move(instance_entity), allocator);
    instance.allocator_ = allocator;

    return Result(Instance(std::move(instance)), result);
}

std::vector<resource::PhysicalDevice> Instance::enum_physical_devices() const {
    uint32_t device_count{};

    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, nullptr), return {};);
    std::vector<VkPhysicalDevice> devices(device_count);
    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, devices.data()), return {};);

    std::vector<resource::PhysicalDevice> device_wrappers(devices.size());

    std::transform(devices.begin(), devices.end(), device_wrappers.begin(), [](auto& d) { return resource::PhysicalDevice(std::move(d)); });

    return device_wrappers;
}

Result<resource::Surface> Instance::create_surface_SDL(SDL_Window* window) const {
    VkSurfaceKHR surface{};
    auto result = SDL_Vulkan_CreateSurface(window, *instance_, &surface) ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;

    return Result(resource::Surface(instance_, std::move(surface), nullptr), result);
}

#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
Result<resource::DebugUtilsMessenger> Instance::create_debug_utils_messenger(VkDebugUtilsMessageSeverityFlagsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const PFN_vkDebugUtilsMessengerCallbackEXT& callback, void* user_data) {
    auto creator = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(*instance_, "vkCreateDebugUtilsMessengerEXT"));
    auto destroyer = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(*instance_, "vkDestroyDebugUtilsMessengerEXT"));

    VkDebugUtilsMessengerCreateInfoEXT messenger_info {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = severity,
        .messageType = type,
        .pfnUserCallback = callback,
        .pUserData = user_data,
    };

    VkDebugUtilsMessengerEXT messenger{};
    auto result = creator(*instance_, &messenger_info, allocator_, &messenger);

    return Result(resource::DebugUtilsMessenger(instance_, std::move(messenger), std::move(destroyer), allocator_), result);
}
#endif

}