#pragma once

#include "../common/common.hpp"
#include "../queue/SubmitInfos.hpp"

namespace vkw {

namespace objects {

class Queue {
    VkQueue queue_;
    uint32_t family_index_;

public:
    using object_type = VkQueue;

    Queue() noexcept {}
    Queue(VkQueue&& queue, uint32_t family_index) noexcept : queue_(queue), family_index_(family_index) {}

    operator VkQueue() const noexcept { return queue_; }

    auto& family_index() const noexcept { return family_index_; }

    VkResult wait() {
        auto res = vkQueueWaitIdle(queue_);

        return res;
    }

    VkResult submit(const queue::SubmitInfos& submit_infos, VkFence fence = VK_NULL_HANDLE) {
        auto res = vkQueueSubmit(queue_, size_u32(submit_infos.infos_.size()), submit_infos.infos_.data(), fence);

        return res;
    }

    std::pair<VkResult, std::vector<VkResult>> present(const std::vector<VkSwapchainKHR>& swapchains, const std::vector<uint32_t>& swapchain_indices, const std::optional<std::vector<VkSemaphore>>& wait_semaphores = std::nullopt) {
        std::vector<VkResult> results(swapchains.size());

        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = wait_semaphores.has_value() ? size_u32(wait_semaphores.value().size()) : 0,
            .pWaitSemaphores = wait_semaphores.has_value() ? wait_semaphores.value().data() : nullptr,
            .swapchainCount = size_u32(swapchains.size()),
            .pSwapchains = swapchains.data(),
            .pImageIndices = swapchain_indices.data(),
            .pResults = results.data(),
        };

        auto res = vkQueuePresentKHR(queue_, &present_info);

        return { res, results };
    }
};

}

}