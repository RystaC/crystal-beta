#pragma once

#include "common.hpp"
#include "PhysicalDevice.hpp"
#include "CommandPool.hpp"
#include "Swapchain.hpp"

namespace vkw {

struct QueueTarget {
    uint32_t family_index;
    std::vector<float> priorities;
};

class Device {
    std::shared_ptr<DeviceEntity> device_;
    std::shared_ptr<PhysicalDevice> physical_device_;
    std::vector<std::vector<VkQueue>> queues_;
    std::vector<uint32_t> queue_family_indices_;

public:
    Device(std::shared_ptr<PhysicalDevice> physical_device) noexcept : device_(nullptr), physical_device_(physical_device), queues_(), queue_family_indices_() {}
    ~Device() noexcept {}

    bool init(const std::vector<QueueTarget>& queue_targets, const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::shared_ptr<CommandPool> create_command_pool(uint32_t queue_family_index = 0);

    std::shared_ptr<Swapchain> create_swapchain(const Surface& surface, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, uint32_t width, uint32_t height, uint32_t queue_family_index = 0);
};

}