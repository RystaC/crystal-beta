#pragma once

#include "common.hpp"
#include "Logger.hpp"

namespace graphics {

class DeviceMemoryAllocator {
    static constexpr VkDeviceSize GENERAL_MEMORY_PAGE_SIZE_ = 1024;
    static constexpr VkDeviceSize TEXTURE_MEMORY_PAGE_SIZE_ = 1024 * 1024;
    static constexpr VkDeviceSize STAGING_MEMORY_PAGE_SIZE_ = 1024;
    vkw::Device& device_;

    std::vector<VkDeviceSize> heap_sizes_;

    std::vector<VkDeviceMemory> general_memory_pool_;
    std::vector<VkDeviceMemory> texture_memory_pool_;
    std::vector<VkDeviceMemory> staging_memory_pool_;

public:
    DeviceMemoryAllocator(vkw::Device& device) noexcept : device_(device) {}
    ~DeviceMemoryAllocator() noexcept {
        for(const auto& memory : general_memory_pool_) {
            device_.free_memory(memory);
        }

        for(const auto& memory : texture_memory_pool_) {
            device_.free_memory(memory);
        }

        for(const auto& memory : staging_memory_pool_) {
            device_.free_memory(memory);
        }
    }
};

}