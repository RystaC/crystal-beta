#pragma once

#include "../common/common.hpp"

namespace vkw {

namespace physical_device {

struct SurfaceProperties {
    VkBool32 supported;
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    SurfaceProperties(uint32_t format_count, uint32_t present_mode_count) : formats(format_count), present_modes(present_mode_count) {}
};

}

}