#pragma once

#include "common.hpp"

namespace vkw {

class Fence {
    std::shared_ptr<DeviceEntity> device_;
    VkFence fence_;

public:
    Fence(std::shared_ptr<DeviceEntity> device, VkFence&& fence) noexcept : device_(device), fence_(fence) {}
};

}