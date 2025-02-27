#pragma once

#include "common.hpp"

namespace graphics {

class Engine {
    static constexpr const char* ENGINE_NAME_ = "crystal-beta";
    // version 0.1.0
    static constexpr uint32_t ENGINE_VERSION_ = VK_MAKE_API_VERSION(0, 0, 1, 0);

    std::unique_ptr<vkw::Instance> instance_;
    std::unique_ptr<vkw::Device> device_;
    vkw::PhysicalDevice physical_device_;
    vkw::Queue queue_;

public:
    static std::unique_ptr<Engine> initialize();
};

}