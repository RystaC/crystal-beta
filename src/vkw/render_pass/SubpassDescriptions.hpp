#pragma once

#include "../common/common.hpp"
#include "AttachmentReferences.hpp"

namespace vkw {

class Device;

namespace render_pass {

class SubpassDescriptions {
    std::vector<VkSubpassDescription> subpasses_;

public:
    friend vkw::Device;

    auto& add(VkPipelineBindPoint bind_point, const std::optional<AttachmentReferences>& input_attachments, const std::optional<AttachmentReferences>& color_attachments, const std::optional<AttachmentReferences>& resolve_attachments, const std::optional<AttachmentReferences>& depth_stencil_attachment, const std::optional<std::vector<uint32_t>>& preserve_attachments) {
        subpasses_.emplace_back(
            VkSubpassDescription {
                .pipelineBindPoint = bind_point,
                .inputAttachmentCount = input_attachments.has_value() ? size_u32(input_attachments.value().refs_.size()) : 0,
                .pInputAttachments = input_attachments.has_value() ? input_attachments.value().refs_.data() : nullptr,
                .colorAttachmentCount = color_attachments.has_value() ? size_u32(color_attachments.value().refs_.size()) : 0,
                .pColorAttachments = color_attachments.has_value() ? color_attachments.value().refs_.data() : nullptr,
                .pResolveAttachments = resolve_attachments.has_value() ? resolve_attachments.value().refs_.data() : nullptr,
                .pDepthStencilAttachment = depth_stencil_attachment.has_value() ? &depth_stencil_attachment.value().refs_[0] : nullptr,
                .preserveAttachmentCount = preserve_attachments.has_value() ? size_u32(preserve_attachments.value().size()) : 0,
                .pPreserveAttachments = preserve_attachments.has_value() ?  preserve_attachments.value().data() : nullptr,
            }
        );

        return *this;
    }
};

}

}