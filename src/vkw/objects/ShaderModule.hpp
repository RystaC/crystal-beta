#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class ShaderModule final {
    std::shared_ptr<objects::Device> device_;
    VkShaderModule shader_module_;

public:
    using object_type = VkShaderModule;

    ShaderModule() noexcept {}
    ShaderModule(std::shared_ptr<objects::Device> device, VkShaderModule&& shader_module) noexcept : device_(device), shader_module_(shader_module) {}
    ~ShaderModule() noexcept {
        vkDestroyShaderModule(*device_, shader_module_, nullptr);
    }
    ShaderModule(const ShaderModule& rhs) = delete;
    auto& operator=(const ShaderModule& rhs) = delete;
    ShaderModule(ShaderModule&& rhs) = default;
    ShaderModule& operator=(ShaderModule&& rhs) = default;

    operator VkShaderModule() const noexcept { return shader_module_; }
};

}

}