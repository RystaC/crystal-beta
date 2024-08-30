#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace render_pass {

class SubpassDependencies {
    std::vector<VkSubpassDependency> dependencies_;

public:
    friend Device;

    auto& add(uint32_t src_index, uint32_t dst_index, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkDependencyFlags dependency_flags) {
        dependencies_.emplace_back(
            VkSubpassDependency {
                .srcSubpass = src_index,
                .dstSubpass = dst_index,
                .srcStageMask = src_stage_mask,
                .dstStageMask = dst_stage_mask,
                .srcAccessMask = src_access_mask,
                .dstAccessMask = dst_access_mask,
                .dependencyFlags = dependency_flags,
            }
        );

        return *this;
    }
};

}

}