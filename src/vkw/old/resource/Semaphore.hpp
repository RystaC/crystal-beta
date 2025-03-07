#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class Semaphore final {
    std::shared_ptr<resource::Device> device_;
    VkSemaphore semaphore_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkSemaphore;

    Semaphore() noexcept {}
    Semaphore(std::shared_ptr<resource::Device> device, VkSemaphore&& semaphore, const VkAllocationCallbacks* allocator) noexcept : device_(device), semaphore_(semaphore), allocator_(allocator) {}
    ~Semaphore() noexcept {
        if(device_) vkDestroySemaphore(*device_, semaphore_, allocator_); 
    }
    Semaphore(const Semaphore& rhs) = delete;
    auto& operator=(const Semaphore& rhs) = delete;
    Semaphore(Semaphore&& rhs) = default;
    Semaphore& operator=(Semaphore&& rhs) = default;

    operator VkSemaphore() const noexcept { return semaphore_; }
};

}

}