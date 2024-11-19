#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class Event final {
    std::shared_ptr<resource::Device> device_;
    VkEvent event_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkEvent;

    Event() noexcept {}
    Event(std::shared_ptr<resource::Device> device, VkEvent&& event, const VkAllocationCallbacks* allocator) noexcept : device_(device), event_(event), allocator_(allocator) {}
    ~Event() noexcept {
        if(device_) vkDestroyEvent(*device_, event_, allocator_);
    }
    Event(const Event& rhs) = delete;
    auto& operator=(const Event& rhs) = delete;
    Event(Event&& rhs) = default;
    Event& operator=(Event&& rhs) = default;

    operator VkEvent() const noexcept { return event_; }

    auto status() const {
        auto status = vkGetEventStatus(*device_, event_);

        return status;
    }
};

}

}