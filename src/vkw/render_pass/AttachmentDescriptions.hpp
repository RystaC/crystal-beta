#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

namespace render_pass {

class AttachmentDescriptions {
    std::vector<VkAttachmentDescription> descs_;

public:
    friend vkw::Device;

    auto& add(VkFormat format, VkSampleCountFlagBits sample_count, VkAttachmentLoadOp loat_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op, VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
        descs_.emplace_back(
            VkAttachmentDescription {
                .format = format,
                .samples = sample_count,
                .loadOp = loat_op,
                .storeOp = store_op,
                .stencilLoadOp = stencil_load_op,
                .stencilStoreOp = stencil_store_op,
                .initialLayout = initial_layout,
                .finalLayout = final_layout,
            }
        );

        return *this;
    }
};

}

}