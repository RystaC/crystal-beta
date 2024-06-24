#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

namespace vkw {

// utility macros
#define CHECK_VK_RESULT(result, stmt) if(result != VK_SUCCESS) { stmt }

// utility functions
inline uint32_t size_u32(size_t s) { return static_cast<uint32_t>(s); }

// wrapper classes for VkInstance and VkDevice
class InstanceEntity {
    VkInstance instance_;

public:
    InstanceEntity(VkInstance&& instance) noexcept : instance_(instance) {}
    ~InstanceEntity() noexcept {
        vkDestroyInstance(instance_, nullptr);
    }

    operator VkInstance() const noexcept { return instance_; }
};

class DeviceEntity {
    VkDevice device_;

public:
    DeviceEntity(VkDevice&& device) noexcept : device_(device) {}
    ~DeviceEntity() noexcept {
        vkDestroyDevice(device_, nullptr);
    }

    operator VkDevice() const noexcept { return device_; }
};

class CommandPool;
class CommandBuffer;

class CommandPoolEntity {
    std::shared_ptr<DeviceEntity> device_;
    VkCommandPool command_pool_;

    friend CommandPool;
    friend CommandBuffer;

public:
    CommandPoolEntity(std::shared_ptr<DeviceEntity> device_, VkCommandPool&& command_pool) noexcept : command_pool_(command_pool) {}
    ~CommandPoolEntity() noexcept {
        vkDestroyCommandPool(*device_, command_pool_, nullptr);
    }

    operator VkCommandPool() const noexcept { return command_pool_; }
};

}