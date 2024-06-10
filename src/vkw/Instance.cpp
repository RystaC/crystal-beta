#include "Instance.hpp"

namespace vkw {

VkResult Instance::init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
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
    auto result = vkCreateInstance(&instance_info, nullptr, &instance);
    if(result != VK_SUCCESS) return result;

    instance_ = std::make_shared<InstanceEntity>(std::move(instance));

    return result;
}

}