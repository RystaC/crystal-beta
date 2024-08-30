#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class DescriptorPool {
    std::shared_ptr<objects::Device> device_;
    VkDescriptorPool pool_;

public:
    DescriptorPool(std::shared_ptr<objects::Device> device, VkDescriptorPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~DescriptorPool() noexcept {
        vkDestroyDescriptorPool(*device_, pool_, nullptr);
    }

    operator VkDescriptorPool() const noexcept { return pool_; }
};

}

}