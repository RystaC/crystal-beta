#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace render_pass {

class SubpassDependencies {
    std::vector<VkSubpassDependency> dependencies_;

public:
    friend Device;

    auto& add(Transition<uint32_t> subpass_transition, Transition<VkPipelineStageFlags> stage_transition, Transition<VkAccessFlags> access_transition, VkDependencyFlags dependency_flags) {
        dependencies_.emplace_back(
            VkSubpassDependency {
                .srcSubpass = subpass_transition.src,
                .dstSubpass = subpass_transition.dst,
                .srcStageMask = stage_transition.src,
                .dstStageMask = stage_transition.dst,
                .srcAccessMask = access_transition.src,
                .dstAccessMask = access_transition.dst,
                .dependencyFlags = dependency_flags,
            }
        );

        return *this;
    }
};

}

}