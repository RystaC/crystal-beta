#pragma once

#include "common.hpp"

namespace vkw {

class ShaderModule {
    std::shared_ptr<DeviceEntity> device_;
    VkShaderModule shader_module_;

public:
    ShaderModule(std::shared_ptr<DeviceEntity> device, VkShaderModule&& shader_module) noexcept : device_(device), shader_module_(shader_module) {}
    ~ShaderModule() noexcept {
        vkDestroyShaderModule(*device_, shader_module_, nullptr);
    }
};

}