#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace resource {

class Instance final {
    VkInstance instance_;

public:
    using resource_type = VkInstance;

    Instance(VkInstance&& instance) noexcept : instance_(instance) {}
    ~Instance() noexcept {
        vkDestroyInstance(instance_, nullptr);
    }
    Instance(const Instance& rhs) = delete;
    auto& operator=(const Instance& rhs) = delete;
    Instance(Instance&& rhs) = default;
    Instance& operator=(Instance&& rhs) = default;

    operator VkInstance() const noexcept { return instance_; }
};

}

}