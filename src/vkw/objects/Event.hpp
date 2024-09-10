#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class Event {
    std::shared_ptr<objects::Device> device_;
    VkEvent event_;

public:
    using object_type = VkEvent;

    Event() noexcept {}
    Event(std::shared_ptr<objects::Device> device, VkEvent&& event) noexcept : device_(device), event_(event) {}
    ~Event() noexcept {
        vkDestroyEvent(*device_, event_, nullptr);
    }

    operator VkEvent() const noexcept { return event_; }

    auto status() const {
        auto status = vkGetEventStatus(*device_, event_);

        return status;
    }
};

}

}