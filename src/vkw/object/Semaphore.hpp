#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace object {

class Semaphore final {
    std::shared_ptr<object::Device> device_;
    VkSemaphore semaphore_;

public:
    using object_type = VkSemaphore;

    Semaphore() noexcept {}
    Semaphore(std::shared_ptr<object::Device> device, VkSemaphore&& semaphore) noexcept : device_(device), semaphore_(semaphore) {}
    ~Semaphore() noexcept {
        if(device_) vkDestroySemaphore(*device_, semaphore_, nullptr); 
    }
    Semaphore(const Semaphore& rhs) = delete;
    auto& operator=(const Semaphore& rhs) = delete;
    Semaphore(Semaphore&& rhs) = default;
    Semaphore& operator=(Semaphore&& rhs) = default;

    operator VkSemaphore() const noexcept { return semaphore_; }
};

}

}