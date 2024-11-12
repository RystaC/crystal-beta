#pragma once

#include "common/common.hpp"
#include "resource/Surface.hpp"
#include "resource/PhysicalDevice.hpp"

namespace vkw {

class Instance {
    std::shared_ptr<resource::Instance> instance_;

    Instance() noexcept {}

public:
    static uint32_t version() {
        uint32_t version{};

        vkEnumerateInstanceVersion(&version);

        return version;
    }

    static std::vector<VkLayerProperties> enumerate_layers() {
        uint32_t count{};
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());

        return layers;
    }

    static std::vector<VkExtensionProperties> enumerate_extensions(const char* layer_name = nullptr) {
        uint32_t count{};
        vkEnumerateInstanceExtensionProperties(layer_name, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateInstanceExtensionProperties(layer_name, &count, extensions.data());

        return extensions;
    }

    static Result<Instance> init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::vector<resource::PhysicalDevice> enum_physical_devices() const;

    Result<resource::Surface> create_surface_SDL(SDL_Window* window) const;
};

}