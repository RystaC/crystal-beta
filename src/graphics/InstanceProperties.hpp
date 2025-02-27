#pragma once

#include "common.hpp"

namespace graphics {

class InstanceProperties {
    uint32_t version_;
    std::vector<VkLayerProperties> layer_properties_;
    std::vector<VkExtensionProperties> extension_properties_;

public:
    InstanceProperties() noexcept {
        vkEnumerateInstanceVersion(&version_);

        {
            uint32_t count{};
            vkEnumerateInstanceLayerProperties(&count, nullptr);
            layer_properties_.resize(count);
            vkEnumerateInstanceLayerProperties(&count, layer_properties_.data());
        }

        {
            // TODO: for layer extensions
            uint32_t count{};
            vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
            extension_properties_.resize(count);
            vkEnumerateInstanceExtensionProperties(nullptr, &count, extension_properties_.data());
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