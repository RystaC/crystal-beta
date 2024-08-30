#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

class Device;

class AttachmentDescriptions {
    std::vector<VkAttachmentDescription> descs_;

public:
    friend Device;

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

class SubpassDescriptions;

class AttachmentReferences {
    std::vector<VkAttachmentReference> refs_;

public:
    friend SubpassDescriptions;

    auto& add(uint32_t attachment_index, VkImageLayout internal_layout) {
        refs_.emplace_back(
            VkAttachmentReference {
                .attachment = attachment_index,
                .layout = internal_layout,
            }
        );

        return *this;
    }
};

class SubpassDescriptions {
    std::vector<VkSubpassDescription> subpasses_;

public:
    friend Device;

    auto& add(VkPipelineBindPoint bind_point, const AttachmentReferences& input_attachments, const AttachmentReferences& color_attachments, const std::optional<AttachmentReferences>& resolve_attachments, const std::optional<VkAttachmentReference>& depth_stencil_attachment, const std::vector<uint32_t>& preserve_attachments) {
        subpasses_.emplace_back(
            VkSubpassDescription {
                .pipelineBindPoint = bind_point,
                .inputAttachmentCount = size_u32(input_attachments.refs_.size()),
                .pInputAttachments = input_attachments.refs_.data(),
                .colorAttachmentCount = size_u32(color_attachments.refs_.size()),
                .pColorAttachments = color_attachments.refs_.data(),
                .pResolveAttachments = resolve_attachments.has_value() ? resolve_attachments.value().refs_.data() : nullptr,
                .pDepthStencilAttachment = depth_stencil_attachment.has_value() ? &depth_stencil_attachment.value() : nullptr,
                .preserveAttachmentCount = size_u32(preserve_attachments.size()),
                .pPreserveAttachments = preserve_attachments.data(),
            }
        );

        return *this;
    }
};

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

namespace objects {

class RenderPass {
    std::shared_ptr<objects::Device> device_;
    VkRenderPass render_pass_;

public:
    RenderPass(std::shared_ptr<objects::Device> device, VkRenderPass&& render_pass) noexcept : device_(device), render_pass_(render_pass) {}
    ~RenderPass() noexcept {
        vkDestroyRenderPass(*device_, render_pass_, nullptr);
    }

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}

}