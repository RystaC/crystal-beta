#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace objects {

class Queue {
    VkQueue queue_;

public:
    using object_type = VkQueue;

    Queue(VkQueue&& queue) noexcept : queue_(queue) {}

    operator VkQueue() const noexcept { return queue_; }
};

}

}