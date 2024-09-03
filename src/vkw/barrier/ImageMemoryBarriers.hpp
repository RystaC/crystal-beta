#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace command_buffer {

class Commands;

}

namespace barrier {

class ImageMemoryBarriers {
    std::vector<VkImageMemoryBarrier> barriers_;

public:
    friend command_buffer::Commands;

    auto& add(VkImage image, VkImageSubresourceRange subresource_range, 
        Transition<VkAccessFlags> access_transition, Transition<VkImageLayout> layout_transition, 
        Transition<uint32_t> queue_family_index_transition = {VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED}
    ) {
        barriers_.emplace_back(
            VkImageMemoryBarrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = access_transition.src,
                .dstAccessMask = access_transition.dst,
                .oldLayout = layout_transition.src,
                .newLayout = layout_transition.dst,
                .srcQueueFamilyIndex = queue_family_index_transition.src,
                .dstQueueFamilyIndex = queue_family_index_transition.dst,
                .image = image,
                .subresourceRange = subresource_range,
            }
        );

        return *this;
    }
};

}

}