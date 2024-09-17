#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace pipeline {

class SpecializationConstants;

class SpecializationMapEntries {
    std::vector<VkSpecializationMapEntry> entries_;

public:
    friend SpecializationConstants;
    SpecializationMapEntries() = default;

    auto& add_entry(uint32_t constant_id, uint32_t offset, size_t size) {
        entries_.emplace_back(
            VkSpecializationMapEntry {
                .constantID = constant_id,
                .offset = offset,
                .size = size,
            }
        );

        return *this;
    }
};

class GraphicsShaderStages;
class ComputeShaderStage;

class SpecializationConstants {
    VkSpecializationInfo specialization_;

public:
    friend GraphicsShaderStages;
    friend ComputeShaderStage;

    template<typename T>
    SpecializationConstants(const SpecializationMapEntries& entries, const std::vector<T>& data) :
    specialization_(
        VkSpecializationInfo {
            .mapEntryCount = size_u32(entries.entries_.size()),
            .pMapEntries = entries.entries_.data(),
            .dataSize = sizeof(T) * data.size(),
            .pData = reinterpret_cast<const void*>(data.data()),
        }
    )
    {}
};

class GraphicsPipelineStates;

class GraphicsShaderStages {
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;

public:
    friend GraphicsPipelineStates;

    auto& vertex_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stages_.emplace_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        );

        return *this;
    }

    auto& fragment_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stages_.emplace_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        );

        return *this;
    }

    auto& geometry_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stages_.emplace_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        );

        return *this;
    }

    auto& tessellation_control_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stages_.emplace_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        );

        return *this;
    }

    auto& tessellation_evaluation_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stages_.emplace_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        );

        return *this;
    }
};

class ComputePipelineStates;

class ComputeShaderStage {
    VkPipelineShaderStageCreateInfo shader_stage_;

public:
    friend ComputePipelineStates;

    void compute_shader(const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") {
        shader_stage_ = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shader_module,
            .pName = entry_point,
            .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
        };
    }
};

}

}