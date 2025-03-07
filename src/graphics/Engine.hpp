#pragma once

#include "common.hpp"
#include "HostMemoryAllocator.hpp"
#include "Logger.hpp"
#include "debug_utils.hpp"

namespace graphics {

class Engine {
    static constexpr const char* ENGINE_NAME_ = "crystal-beta";
    // version 0.1.0
    static constexpr uint32_t ENGINE_VERSION_ = VK_MAKE_API_VERSION(0, 0, 1, 0);

    vkw::Instance instance_;
    vkw::Device device_;
    vkw::Queue queue_;

#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger_;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger_;
    VkDebugUtilsMessengerEXT debug_utils_messenger_;
#endif

public:
    ~Engine() noexcept {
#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
            destroy_debug_utils_messenger_(*instance_, debug_utils_messenger_, HostMemoryAllocator::get());
#endif
        }

    static std::unique_ptr<Engine> initialize();
};

}