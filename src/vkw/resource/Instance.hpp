#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace resource {

class Instance final {
    VkInstance instance_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkInstance;

    Instance(VkInstance&& instance, const VkAllocationCallbacks* allocator) noexcept : instance_(instance), allocator_(allocator) {}
    ~Instance() noexcept {
        vkDestroyInstance(instance_, allocator_);
    }
    Instance(const Instance& rhs) = delete;
    auto& operator=(const Instance& rhs) = delete;
    Instance(Instance&& rhs) = default;
    Instance& operator=(Instance&& rhs) = default;

    operator VkInstance() const noexcept { return instance_; }
};

}

}