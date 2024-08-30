#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace render_pass {

class SubpassDescriptions;

class AttachmentReferences {
    std::vector<VkAttachmentReference> refs_;

public:
    friend SubpassDescriptions;

    auto& add(uint32_t attachment_index, VkImageLayout internal_layout = VK_IMAGE_LAYOUT_UNDEFINED) {
        refs_.emplace_back(
            VkAttachmentReference {
                .attachment = attachment_index,
                .layout = internal_layout,
            }
        );

        return *this;
    }
};


}

}