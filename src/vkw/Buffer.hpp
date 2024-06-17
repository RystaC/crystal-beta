#pragma once

#include "common.hpp"

namespace vkw {

class Buffer {
    std::shared_ptr<DeviceEntity> device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;
};

}