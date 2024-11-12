#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class QueryPool final {
    std::shared_ptr<resource::Device> device_;
    VkQueryPool pool_;

public:
    using resource_type = VkQueryPool;

    QueryPool() noexcept {}
    QueryPool(std::shared_ptr<resource::Device> device, VkQueryPool&& pool) noexcept : device_(device), pool_(pool) {}
    ~QueryPool() noexcept {
        if(device_) vkDestroyQueryPool(*device_, pool_, nullptr);
    }
    QueryPool(const QueryPool& rhs) = delete;
    auto& operator=(const QueryPool& rhs) = delete;
    QueryPool(QueryPool&& rhs) = default;
    QueryPool& operator=(QueryPool&& rhs) = default;

    operator VkQueryPool() const noexcept { return pool_; }
};

}

}