#pragma once

#include "common.hpp"
#include "Logger.hpp"

namespace graphics {

#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
inline VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    std::string type_str{};

    if(type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type_str += " GENERAL ";
    }
    if(type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type_str += " VALIDATION ";
    }
    if(type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type_str += " PERFORMANCE ";
    }
    if(type & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        type_str += " DEVICE_ADDRESS_BINDING ";
    }

    switch(severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(std::format("({}) {}", type_str, callback_data->pMessage));
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(std::format("({}) {}", type_str, callback_data->pMessage));
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(std::format("({}) {}", type_str, callback_data->pMessage));
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_DEBUG(std::format("({}) {}", type_str, callback_data->pMessage));
            break;
        default:
            break;
    }
    
    // should return VK_FALSE for user defined callback
    return VK_FALSE;
}
#endif

}