#pragma once

#include "common.hpp"

namespace graphics {

constexpr std::pair<const char*, const char*> BLACK_FRONT_STR = std::make_pair("\x1b[30m", "\x1b[39m");
constexpr std::pair<const char*, const char*> RED_FRONT_STR = std::make_pair("\x1b[31m", "\x1b[39m");
constexpr std::pair<const char*, const char*> GREEN_FRONT_STR = std::make_pair("\x1b[32m", "\x1b[39m");
constexpr std::pair<const char*, const char*> YELLOW_FRONT_STR = std::make_pair("\x1b[33m", "\x1b[39m");
constexpr std::pair<const char*, const char*> BLUE_FRONT_STR = std::make_pair("\x1b[34m", "\x1b[39m");
constexpr std::pair<const char*, const char*> MAGENTA_FRONT_STR = std::make_pair("\x1b[35m", "\x1b[39m");
constexpr std::pair<const char*, const char*> CYAN_FRONT_STR = std::make_pair("\x1b[36m", "\x1b[39m");
constexpr std::pair<const char*, const char*> GREY_FRONT_STR = std::make_pair("\x1b[37m", "\x1b[39m");

constexpr std::pair<const char*, const char*> BLACK_BACK_STR = std::make_pair("\x1b[40m", "\x1b[49m");
constexpr std::pair<const char*, const char*> RED_BACK_STR = std::make_pair("\x1b[41m", "\x1b[49m");
constexpr std::pair<const char*, const char*> GREEN_BACK_STR = std::make_pair("\x1b[42m", "\x1b[49m");
constexpr std::pair<const char*, const char*> YELLOW_BACK_STR = std::make_pair("\x1b[43m", "\x1b[49m");
constexpr std::pair<const char*, const char*> BLUE_BACK_STR = std::make_pair("\x1b[44m", "\x1b[49m");
constexpr std::pair<const char*, const char*> MAGENTA_BACK_STR = std::make_pair("\x1b[45m", "\x1b[49m");
constexpr std::pair<const char*, const char*> CYAN_BACK_STR = std::make_pair("\x1b[46m", "\x1b[49m");
constexpr std::pair<const char*, const char*> GREY_BACK_STR = std::make_pair("\x1b[47m", "\x1b[49m");

#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    std::string severity_str{}, type_str{}, color_head{}, color_tail{};

    switch(severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity_str = "ERROR";
            color_head = "\x1b[31m";
            color_tail = "\x1b[39m";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity_str = "WARNING";
            color_head = "\x1b[33m";
            color_tail = "\x1b[39m";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity_str = "INFO";
            color_head = "\x1b[36m";
            color_tail = "\x1b[39m";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            severity_str = "VERBOSE";
            color_head = "\x1b[37m";
            color_tail = "\x1b[39m";
            break;
        default:
            break;
    }

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

    // current time
    auto time_now = std::chrono::system_clock::now();
    // cast time_t for strftime
    auto time_sec = std::chrono::system_clock::to_time_t(time_now);
    // cast for miliseconds
    auto time_msec = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
    // convert to local time
    auto time_local = std::localtime(&time_sec);

    std::array<char, 32> time_str{};
    std::strftime(time_str.data(), time_str.size(), "%Y/%m/%d %H:%M:%S", time_local);
    // extract lowest 3 digits
    auto time_msec_val = time_msec.count() % 1000;

    std::cerr << std::format("{}<{}.{}>[{}]({}): message id = {}, id = {}, {}{}", color_head, time_str.data(), time_msec_val, severity_str, type_str, callback_data->pMessageIdName, callback_data->messageIdNumber, callback_data->pMessage, color_tail) << std::endl;

    // should return VK_FALSE for user defined callback
    return VK_FALSE;
}
#endif

}