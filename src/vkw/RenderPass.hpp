#pragma once

#include "common.hpp"

namespace vkw {

class Device;

class RenderPassGraph {
    std::vector<VkAttachmentDescription> attachments_;
    std::vector<VkSubpassDescription> subpasses_;
    std::vector<VkSubpassDependency> dependencies_;

public:
    friend Device;
    
    RenderPassGraph() = default;
    ~RenderPassGraph() = default;

    void add_attachment(VkFormat format, VkSampleCountFlagBits sample_count, VkAttachmentLoadOp loat_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op, VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout) {
        
        attachments_.emplace_back(
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
    }

    void add_subpass(VkPipelineBindPoint bind_point, const std::vector<VkAttachmentReference>& input_attachments, const std::vector<VkAttachmentReference>& color_attachments, const std::vector<VkAttachmentReference>& resolve_attachments, const std::optional<VkAttachmentReference>& depth_stencil_attachment, const std::vector<uint32_t>& preserve_attachments) {
        subpasses_.emplace_back(
            VkSubpassDescription {
                .pipelineBindPoint = bind_point,
                .inputAttachmentCount = size_u32(input_attachments.size()),
                .pInputAttachments = input_attachments.data(),
                .colorAttachmentCount = size_u32(color_attachments.size()),
                .pColorAttachments = color_attachments.data(),
                .pResolveAttachments = resolve_attachments.empty() ? nullptr : resolve_attachments.data(),
                .pDepthStencilAttachment = depth_stencil_attachment.has_value() ? &depth_stencil_attachment.value() : nullptr,
                .preserveAttachmentCount = size_u32(preserve_attachments.size()),
                .pPreserveAttachments = preserve_attachments.data(),
            }
        );
    }

    void add_dependency(uint32_t src_index, uint32_t dst_index, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask, VkAccessFlags src_access_mask, VkAccessFlags dst_access_mask, VkDependencyFlags dependency_flags) {
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
    }
};

class RenderPass {
    std::shared_ptr<DeviceEntity> device_;
    VkRenderPass render_pass_;

public:
    RenderPass(std::shared_ptr<DeviceEntity> device, VkRenderPass&& render_pass) noexcept : device_(device), render_pass_(render_pass) {}
    ~RenderPass() noexcept {
        vkDestroyRenderPass(*device_, render_pass_, nullptr);
    }

    operator VkRenderPass() const noexcept { return render_pass_; }
};

}