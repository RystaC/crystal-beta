#pragma once

#include "common.hpp"

namespace graphics {

class DeviceProperties {
    std::vector<VkLayerProperties> layer_properties_;
    std::vector<VkExtensionProperties> extension_properties_;

public:
    DeviceProperties(const VkPhysicalDevice& physical_device) noexcept {
        {
            uint32_t count{};
            vkEnumerateDeviceLayerProperties(physical_device, &count, nullptr);
            layer_properties_.resize(count);
            vkEnumerateDeviceLayerProperties(physical_device, &count, layer_properties_.data());
        }

        {
            uint32_t count{};
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, nullptr);
            extension_properties_.resize(count);
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, extension_properties_.data());
        }
    }

    bool find_layer(const char* layer_name) const noexcept {
        for(const auto& prop : layer_properties_) {
            if(std::strcmp(prop.layerName, layer_name) == 0) {
                return true;
            }
        }

        return false;
    }

    bool find_extension(const char* extension_name) const noexcept {
        for(const auto& prop : extension_properties_) {
            if(std::strcmp(prop.extensionName, extension_name) == 0) {
                return true;
            }
        }

        return false;
    }
};

}