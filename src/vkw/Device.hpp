#pragma once

#include "common.hpp"

namespace vkw {

class Device {
    std::shared_ptr<DeviceEntity> device_;

public:
    Device() noexcept : device_(nullptr) {}
    ~Device() noexcept {}

    bool init(const VkPhysicalDevice& physical_device, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

};

}