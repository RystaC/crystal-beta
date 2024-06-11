#pragma once

#include "common.hpp"
#include "Surface.hpp"

namespace vkw {

class Instance {
    std::shared_ptr<InstanceEntity> instance_;

public:
    Instance() noexcept : instance_() {}
    ~Instance() noexcept {}

    bool init(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

    std::vector<VkPhysicalDevice> enum_physical_devices() const;

    std::unique_ptr<Surface> create_surface_SDL(SDL_Window* window) const;
};

}