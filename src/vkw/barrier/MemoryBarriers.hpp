#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace command_buffer {

class Commands;

}

namespace barrier {

class MemoryBarriers {
    std::vector<VkMemoryBarrier> barriers_;

public:
    friend command_buffer::Commands;

    auto& add(Transition<VkAccessFlags> access_transition) {
        barriers_.emplace_back(
            VkMemoryBarrier {
                .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
                .srcAccessMask = access_transition.src,
                .dstAccessMask = access_transition.dst,
            }
        );

        return *this;
    }

};

}

}