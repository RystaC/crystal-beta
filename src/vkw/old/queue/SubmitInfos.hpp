#pragma once

#include "../common/common.hpp"

#include "../resource/CommandBuffers.hpp"

namespace vkw {

namespace resource {

class Queue;

}

namespace queue {

class SubmitInfos {
    std::vector<VkSubmitInfo> infos_;

public:
    friend vkw::resource::Queue;

    auto& add(const resource::CommandBuffers& command_buffers, const std::optional<std::vector<VkSemaphore>>& wait_semaphores = std::nullopt, const std::optional<std::vector<VkPipelineStageFlags>>& wait_dst_stages = std::nullopt, const std::optional<std::vector<VkSemaphore>>& signal_semaphores = std::nullopt) {
        uint32_t wait_count = wait_semaphores.has_value() ? size_u32(wait_semaphores.value().size()) : 0;
        const VkSemaphore* wait_sem = wait_semaphores.has_value() ? wait_semaphores.value().data() : nullptr;
        uint32_t sig_count = signal_semaphores.has_value() ? size_u32(signal_semaphores.value().size()) : 0;
        const VkSemaphore* sig_sem = signal_semaphores.has_value() ? signal_semaphores.value().data() : nullptr;

        infos_.emplace_back(
            VkSubmitInfo {
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = wait_count,
                .pWaitSemaphores = wait_sem,
                .pWaitDstStageMask = wait_dst_stages.has_value() ? wait_dst_stages.value().data() : nullptr,
                .commandBufferCount = size_u32(command_buffers.size()),
                .pCommandBuffers = command_buffers.command_buffers_.data(),
                .signalSemaphoreCount = sig_count,
                .pSignalSemaphores = sig_sem,
            }
        );

        return *this;
    }
};

}

}