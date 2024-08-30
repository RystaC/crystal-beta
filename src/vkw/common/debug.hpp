#pragma once

#include "common.hpp"

namespace vkw {

#define FMT(...) std::format(__VA_ARGS__)

#define PRINT_VK_RESULT(result) printf("%s\n", result_to_str(result))

inline void log_impl(const std::string& content, std::ostream& os = std::cerr) {
    os << content << std::endl;
}

inline constexpr const char* result_to_str(VkResult result) {
#define STR(s) #s
    switch(result) {
        case VK_SUCCESS:
            return STR(VK_SUCCESS);
        case VK_NOT_READY:
            return STR(VK_NOT_READY);
        case VK_TIMEOUT:
            return STR(VK_TIMEOUT);
        case VK_EVENT_SET:
            return STR(VK_EVENT_SET);
        case VK_EVENT_RESET:
            return STR(VK_EVENT_RESET);
        case VK_INCOMPLETE:
            return STR(VK_INCOMPLETE);
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        case VK_ERROR_INITIALIZATION_FAILED:
            return STR(VK_ERROR_INITIALIZATION_FAILED);
        case VK_ERROR_DEVICE_LOST:
            return STR(VK_ERROR_DEVICE_LOST);
        case VK_ERROR_MEMORY_MAP_FAILED:
            return STR(VK_ERROR_MEMORY_MAP_FAILED);
        case VK_ERROR_LAYER_NOT_PRESENT:
            return STR(VK_ERROR_LAYER_NOT_PRESENT);
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return STR(VK_ERROR_FEATURE_NOT_PRESENT);
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return STR(VK_ERROR_INCOMPATIBLE_DRIVER);
        case VK_ERROR_TOO_MANY_OBJECTS:
            return STR(VK_ERROR_TOO_MANY_OBJECTS);
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return STR(VK_ERROR_FORMAT_NOT_SUPPORTED);
        case VK_ERROR_FRAGMENTED_POOL:
            return STR(VK_ERROR_FRAGMENTED_POOL);

#if defined(VK_VERSION_1_1)
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return STR(VK_ERROR_OUT_OF_POOL_MEMORY);
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return STR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);
#endif

#if defined(VK_VERSION_1_2)
        case VK_ERROR_FRAGMENTATION:
            return STR(VK_ERROR_FRAGMENTATION);
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
#endif

#if defined(VK_VERSION_1_3)
        case VK_PIPELINE_COMPILE_REQUIRED:
            return STR(VK_PIPELINE_COMPILE_REQUIRED);
#endif

#if defined(VK_KHR_surface)
        case VK_ERROR_SURFACE_LOST_KHR:
            return STR(VK_ERROR_SURFACE_LOST_KHR);
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
#endif

#if defined(VK_KHR_swapchain)
        case VK_SUBOPTIMAL_KHR:
            return STR(VK_SUBOPTIMAL_KHR);
        case VK_ERROR_OUT_OF_DATE_KHR:
            return STR(VK_ERROR_OUT_OF_DATE_KHR);
#endif

#if defined(VK_KHR_display_swapchain)
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
#endif

#if defined(VK_EXT_debug_report)
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return STR(VK_ERROR_VALIDATION_FAILED_EXT);
#endif

#if defined(VK_NV_glsl_shader)
        case VK_ERROR_INVALID_SHADER_NV:
            return STR(VK_ERROR_INVALID_SHADER_NV);
#endif

#if defined(VK_KHR_video_queue)
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
            return STR(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
#endif

#if defined(VK_EXT_image_drm_format_modifier)
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
#endif

#if defined(VK_KHR_global_priority)
        case VK_ERROR_NOT_PERMITTED_KHR:
            return STR(VK_ERROR_NOT_PERMITTED_KHR);
#endif

#if defined(VK_EXT_full_screen_exclusive)
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
#endif

#if defined(VK_KHR_deferred_host_operations)
        case VK_THREAD_IDLE_KHR:
            return STR(VK_THREAD_IDLE_KHR);
        case VK_THREAD_DONE_KHR:
            return STR(VK_THREAD_DONE_KHR);
        case VK_OPERATION_DEFERRED_KHR:
            return STR(VK_OPERATION_DEFERRED_KHR);
        case VK_OPERATION_NOT_DEFERRED_KHR:
            return STR(VK_OPERATION_NOT_DEFERRED_KHR);
#endif

#if defined(VK_KHR_video_encode_queue)
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
            return STR(VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
#endif

#if defined(VK_EXT_image_compression_control)
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
            return STR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
#endif

#if defined(VK_EXT_shader_object)
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
            return STR(VK_INCOMPATIBLE_SHADER_BINARY_EXT);
#endif

#if defined(VK_KHR_maintenance1) && !defined(VK_VERSION_1_1)
        case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
            return STR(VK_ERROR_OUT_OF_POOL_MEMORY_KHR);
#endif

#if defined(VK_KHR_external_memory) && !defined(VK_VERSION_1_1)
        case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
            return STR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR);
#endif

#if defined(VK_EXT_descriptor_indexing) && !defined(VK_VERSION_1_2)
        case VK_ERROR_FRAGMENTATION_EXT:
            return STR(VK_ERROR_FRAGMENTATION_EXT);
#endif

#if defined(VK_EXT_global_priority) && !defined(VK_KHR_global_priority)
        case VK_ERROR_NOT_PERMITTED_EXT:
            return STR(VK_ERROR_NOT_PERMITTED_EXT);
#endif

#if defined(VK_EXT_buffer_device_address) && !defined(VK_VERSION_1_2)
        case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
            return STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);

#endif

#if defined(VK_KHR_buffer_device_address) && !defined(VK_VERSION_1_2)
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
            return STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR);
#endif

#if defined(VK_EXT_pipeline_creation_cache_control) && !defined(VK_VERSION_1_3)
        case VK_PIPELINE_COMPILE_REQUIRED_EXT:
            return STR(VK_PIPELINE_COMPILE_REQUIRED_EXT);
        case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT:
            return STR(VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT);
#endif

#if defined(VK_EXT_shader_object) && !defined(VK_EXT_shader_object)
        case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
            return STR(VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
#endif

        default:
            return STR(VK_ERROR_UNKNOWN);
    }
#undef STR
}

}