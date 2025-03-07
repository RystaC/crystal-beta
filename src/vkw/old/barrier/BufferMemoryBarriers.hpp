#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace command_buffer {

class Commands;

}

namespace barrier {

class BufferMemoryBarriers {
    std::vector<VkBufferMemoryBarrier> barriers_;

public:
    friend command_buffer::Commands;

    auto& add(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size, Transition<VkAccessFlags> access_transition, Transition<uint32_t> queue_family_index_transition = {VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED}) {
        barriers_.emplace_back(
            VkBufferMemoryBarrier {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask = access_transition.src,
                .dstAccessMask = access_transition.dst,
                .srcQueueFamilyIndex = queue_family_index_transition.src,
                .dstQueueFamilyIndex = queue_family_index_transition.dst,
                .buffer = buffer,
                .offset = offset,
                .size = size,
            }
        );

        return *this;
    }
};

}

}