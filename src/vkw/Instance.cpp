#include "Instance.hpp"

namespace vkw {

bool Instance::init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
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

    VkInstance instance{};
    CHECK_VK_RESULT(vkCreateInstance(&instance_info, nullptr, &instance), return false;);

    instance_ = std::make_shared<InstanceEntity>(std::move(instance));

    return true;
}

std::vector<PhysicalDevice> Instance::enum_physical_devices() const {
    uint32_t device_count{};

    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, nullptr), return {};);
    std::vector<VkPhysicalDevice> devices(device_count);
    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, devices.data()), return {};);

    std::vector<PhysicalDevice> device_wrappers(devices.size());

    std::transform(devices.begin(), devices.end(), device_wrappers.begin(), [](auto& d) { return PhysicalDevice(std::move(d)); });

    return device_wrappers;
}

std::unique_ptr<Surface> Instance::create_surface_SDL(SDL_Window* window) const {
    VkSurfaceKHR surface{};
    auto result = SDL_Vulkan_CreateSurface(window, *instance_, &surface);
    if(result != SDL_TRUE) return {};

    return std::make_unique<Surface>(instance_, std::move(surface));
}

}