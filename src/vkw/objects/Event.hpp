#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Event final {
    std::shared_ptr<objects::Device> device_;
    VkEvent event_;

public:
    using object_type = VkEvent;

    Event() noexcept {}
    Event(std::shared_ptr<objects::Device> device, VkEvent&& event) noexcept : device_(device), event_(event) {}
    ~Event() noexcept {
        if(device_) vkDestroyEvent(*device_, event_, nullptr);
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