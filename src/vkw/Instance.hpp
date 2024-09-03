#pragma once

#include "common/common.hpp"
#include "objects/Surface.hpp"
#include "objects/PhysicalDevice.hpp"

namespace vkw {

class Instance {
    std::shared_ptr<objects::Instance> instance_;

public:
    Instance() noexcept : instance_(nullptr) {}
    ~Instance() noexcept {}

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

    bool init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::vector<objects::PhysicalDevice> enum_physical_devices() const;

    objects::Surface create_surface_SDL(SDL_Window* window) const;
};

}