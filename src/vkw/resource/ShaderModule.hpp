#pragma once

#include "../common/common.hpp"
#include "Device.hpp"

namespace vkw {

namespace resource {

class ShaderModule final {
    std::shared_ptr<resource::Device> device_;
    VkShaderModule shader_module_;
    const VkAllocationCallbacks* allocator_;

public:
    using resource_type = VkShaderModule;

    ShaderModule() noexcept {}
    ShaderModule(std::shared_ptr<resource::Device> device, VkShaderModule&& shader_module, const VkAllocationCallbacks* allocator) noexcept : device_(device), shader_module_(shader_module), allocator_(allocator) {}
    ~ShaderModule() noexcept {
        if(device_) vkDestroyShaderModule(*device_, shader_module_, allocator_);
    }
    ShaderModule(const ShaderModule& rhs) = delete;
    auto& operator=(const ShaderModule& rhs) = delete;
    ShaderModule(ShaderModule&& rhs) = default;
    ShaderModule& operator=(ShaderModule&& rhs) = default;

    operator VkShaderModule() const noexcept { return shader_module_; }
};

}

}