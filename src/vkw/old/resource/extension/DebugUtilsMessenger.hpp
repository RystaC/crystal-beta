#pragma once

#include "../../common/common.hpp"

#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)

#include "../Instance.hpp"

namespace vkw {

class Instance;

namespace resource {

class DebugUtilsMessenger final {
    std::shared_ptr<resource::Instance> instance_;
    VkDebugUtilsMessengerEXT messenger_;
    PFN_vkDestroyDebugUtilsMessengerEXT destroyer_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkDebugUtilsMessengerEXT;

    friend vkw::Instance;

    DebugUtilsMessenger() noexcept {}
    DebugUtilsMessenger(std::shared_ptr<resource::Instance> instance, VkDebugUtilsMessengerEXT&& messenger, PFN_vkDestroyDebugUtilsMessengerEXT&& destoryer, const VkAllocationCallbacks* allocator) noexcept : instance_(instance), messenger_(messenger), destroyer_(destoryer), allocator_(allocator) {}
    ~DebugUtilsMessenger() noexcept {
        if(instance_) {
            destroyer_(*instance_, messenger_, allocator_);
        }
    }
    DebugUtilsMessenger(const DebugUtilsMessenger& rhs) = delete;
    auto& operator=(const DebugUtilsMessenger& rhs) = delete;
    DebugUtilsMessenger(DebugUtilsMessenger&& rhs) = default;
    DebugUtilsMessenger& operator=(DebugUtilsMessenger&& rhs) = default;

    operator VkDebugUtilsMessengerEXT() const noexcept { return messenger_; }
};

}

}

#endif