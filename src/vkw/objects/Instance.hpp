#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace objects {

class Instance {
    VkInstance instance_;

public:
    using object_type = VkInstance;

    Instance(VkInstance&& instance) noexcept : instance_(instance) {}
    ~Instance() noexcept {
        vkDestroyInstance(instance_, nullptr);
    }

    operator VkInstance() const noexcept { return instance_; }
};

}

}