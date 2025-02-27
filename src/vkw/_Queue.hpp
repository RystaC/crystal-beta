#pragma once

#include "common.hpp"

namespace vkw {

class Queue {
    const VkQueue& queue_;
    uint32_t family_index_;
    uint32_t queue_index_;

public:
    Queue(const VkQueue& queue, uint32_t family_index, uint32_t queue_index) noexcept : queue_(queue), family_index_(family_index), queue_index_(queue_index) {}
    ~Queue() noexcept = default;

    operator VkQueue() const noexcept { return queue_; } 

    auto family_index() const noexcept { return family_index_; }
    auto queue_index() const noexcept { return queue_index_; }

    auto submit(const std::vector<VkSubmitInfo2>& submit_info, const VkFence& fence = VK_NULL_HANDLE) {
        return vkQueueSubmit2(queue_, size_u32(submit_info), submit_info.data(), fence);
    }

    auto wait_idle() {
        return vkQueueWaitIdle(queue_);
    }

#if defined(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    auto present(const VkPresentInfoKHR& present_info) {
        return vkQueuePresentKHR(queue_, &present_info);
    }
#endif

};

}