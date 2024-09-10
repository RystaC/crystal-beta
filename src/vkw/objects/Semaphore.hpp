#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Semaphore {
    std::shared_ptr<objects::Device> device_;
    VkSemaphore semaphore_;

public:
    using object_type = VkSemaphore;

    Semaphore() noexcept {}
    Semaphore(std::shared_ptr<objects::Device> device, VkSemaphore&& semaphore) noexcept : device_(device), semaphore_(semaphore) {}
    ~Semaphore() noexcept {
        vkDestroySemaphore(*device_, semaphore_, nullptr); 
    }

    operator VkSemaphore() const noexcept { return semaphore_; }
};

}

}