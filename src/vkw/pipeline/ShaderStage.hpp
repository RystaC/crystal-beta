#pragma once

#include "../common/common.hpp"

namespace vkw {

class Device;

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

class ShaderStage;

class SpecializationConstants {
    VkSpecializationInfo specialization_;

public:
    friend ShaderStage;

    template<typename T>
    SpecializationConstants(const SpecializationMapEntries& entries, const std::vector<T>& data) :
    specialization_(
        VkSpecializationInfo {
            .mapEntryCount = size_u32(entries.entries_.size()),
            .pMapEntries = entries.entries_.data(),
            .dataSize = sizeof(T) * data.size(),
            .pData = reinterpret_cast<void*>(data.data()),
        }
    )
    {}
};

class ShaderStage {
    VkPipelineShaderStageCreateInfo stage_;

public:
    ShaderStage(VkShaderStageFlagBits stage, const VkShaderModule& shader_module, const std::optional<SpecializationConstants>& specialization_constants = std::nullopt, const char* entry_point = "main") :
        stage_(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = stage,
                .module = shader_module,
                .pName = entry_point,
                .pSpecializationInfo = specialization_constants.has_value() ? &specialization_constants.value().specialization_ : nullptr,
            }
        ) {}
};

}

}