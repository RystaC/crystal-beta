#include "Instance.hpp"

namespace vkw {

Result<Instance> Instance::init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
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
    auto result = vkCreateInstance(&instance_info, nullptr, &instance_entity);

    instance.instance_ = std::make_shared<object::Instance>(std::move(instance_entity));

    return Result(Instance(std::move(instance)), result);
}

std::vector<object::PhysicalDevice> Instance::enum_physical_devices() const {
    uint32_t device_count{};

    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, nullptr), return {};);
    std::vector<VkPhysicalDevice> devices(device_count);
    CHECK_VK_RESULT(vkEnumeratePhysicalDevices(*instance_, &device_count, devices.data()), return {};);

    std::vector<object::PhysicalDevice> device_wrappers(devices.size());

    std::transform(devices.begin(), devices.end(), device_wrappers.begin(), [](auto& d) { return object::PhysicalDevice(std::move(d)); });

    return device_wrappers;
}

Result<object::Surface> Instance::create_surface_SDL(SDL_Window* window) const {
    VkSurfaceKHR surface{};
    auto result = SDL_Vulkan_CreateSurface(window, *instance_, &surface) ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;

    return Result(object::Surface(instance_, std::move(surface)), result);
}

}