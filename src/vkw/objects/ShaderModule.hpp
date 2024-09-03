#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace objects {

class ShaderModule {
    std::shared_ptr<objects::Device> device_;
    VkShaderModule shader_module_;

public:
    using object_type = VkShaderModule;

    ShaderModule(std::shared_ptr<objects::Device> device, VkShaderModule&& shader_module) noexcept : device_(device), shader_module_(shader_module) {}
    ~ShaderModule() noexcept {
        vkDestroyShaderModule(*device_, shader_module_, nullptr);
    }

    operator VkShaderModule() const noexcept { return shader_module_; }
};

}

}