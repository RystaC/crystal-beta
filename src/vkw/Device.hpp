#pragma once

#include "common/common.hpp"

#include "objects/Device.hpp"
#include "objects/PhysicalDevice.hpp"

#include "queue/CreateInfos.hpp"

namespace vkw {

class Device {
    std::shared_ptr<objects::Device> device_;
    objects::PhysicalDevice& physical_device_;

    std::unordered_map<uint32_t, uint32_t> queue_counts_;

public:
    Device(objects::PhysicalDevice& p_device) noexcept : physical_device_(p_device) {}

    static std::vector<VkLayerProperties> enumerate_layers(objects::PhysicalDevice p_device) {
        uint32_t count{};
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateDeviceLayerProperties(p_device.physical_device_, &count, layers.data());

        return layers;
    }

    static std::vector<VkExtensionProperties> enumerate_extensions(objects::PhysicalDevice p_device, const char* layer_name = nullptr) {
        uint32_t count{};
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, nullptr);
        std::vector<VkExtensionProperties> extensions(count);
        vkEnumerateDeviceExtensionProperties(p_device.physical_device_, layer_name, &count, extensions.data());

        return extensions;
    }

    bool init(const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);
};

}