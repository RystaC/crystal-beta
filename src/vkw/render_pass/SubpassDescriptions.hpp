#pragma once

#include "../common/common.hpp"
#include "AttachmentReferences.hpp"

namespace vkw {

class Device;

namespace render_pass {

class SubpassDescription_ {
    VkSubpassDescription desc_;
    std::vector<VkSubpassDescription>& descs_;

public:
    SubpassDescription_(std::vector<VkSubpassDescription>& descs) noexcept : desc_{.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS}, descs_(descs) {}

    [[nodiscard]]
    auto& input_attachments(const AttachmentReferences& attachments) {
        desc_.inputAttachmentCount = size_u32(attachments.refs_.size());
        desc_.pInputAttachments = attachments.refs_.data();

        return *this;
    }

    [[nodiscard]]
    auto& output_color_attachments(const AttachmentReferences& attachments) {
        desc_.colorAttachmentCount = size_u32(attachments.refs_.size());
        desc_.pColorAttachments = attachments.refs_.data();

        return *this;
    }

    [[nodiscard]]
    auto& output_resolve_attachments(const AttachmentReferences& attachments) {
        desc_.pResolveAttachments = attachments.refs_.data();

        return *this;
    }

    [[nodiscard]]
    auto& output_depth_attachment(const AttachmentReferences& attachment) {
        desc_.pDepthStencilAttachment = &attachment.refs_[0];

        return *this;
    }

    [[nodiscard]]
    auto& preserve_attachments(const std::vector<uint32_t>& attachment_indices) {
        desc_.preserveAttachmentCount = size_u32(attachment_indices.size());
        desc_.pPreserveAttachments = attachment_indices.data();

        return *this;
    }

    void end() {
        descs_.emplace_back(desc_);
    }
};

class SubpassDescriptions {
    public:
    std::vector<VkSubpassDescription> subpasses_;

public:
    friend vkw::Device;

    SubpassDescription_ add() {
        return SubpassDescription_(subpasses_);
    }
};

}

}