#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class QueryPool {
    std::shared_ptr<objects::Device> device_;
    VkQueryPool pool_;

public:
    using object_type = VkQueryPool;

    QueryPool(std::shared_ptr<objects::Device> device, VkQueryPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~QueryPool() noexcept {
        vkDestroyQueryPool(*device_, pool_, nullptr);
    }

    operator VkQueryPool() const noexcept { return pool_; }
};

}

}