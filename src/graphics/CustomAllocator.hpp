#pragma once

#include "common.hpp"

namespace graphics {

class CustomAllocator {
    VkAllocationCallbacks callbacks_;

    static const char* get_scope_str_(VkSystemAllocationScope scope) {
        switch(scope) {
            case VK_SYSTEM_ALLOCATION_SCOPE_COMMAND:
                return "COMMAND";
            case VK_SYSTEM_ALLOCATION_SCOPE_OBJECT:
                return "OBJECT";
            case VK_SYSTEM_ALLOCATION_SCOPE_CACHE:
                return "CACHE";
            case VK_SYSTEM_ALLOCATION_SCOPE_DEVICE:
                return "DEVICE";
            case VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE:
                return "INSTANCE";
            default:
                return "UNKNOWN";
        }
    }

    static void* VKAPI_CALL allocation_(void* user_data, size_t size, size_t alignment, VkSystemAllocationScope scope) {
        auto p = _aligned_malloc(size, alignment);
        return p;
    }

    static void* VKAPI_CALL reallocation_(void* user_data, void* original, size_t size, size_t alignment, VkSystemAllocationScope scope) {
        auto p = _aligned_realloc(original, size, alignment);
        return p;
    }

    static void VKAPI_CALL free_(void* user_data, void* memory) {
        _aligned_free(memory);
    }

    static void VKAPI_CALL allocation_nortification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {

    }

    static void VKAPI_CALL free_notification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {

    }

public:
    CustomAllocator() noexcept : callbacks_ {
        .pUserData = nullptr,
        .pfnAllocation = reinterpret_cast<PFN_vkAllocationFunction>(allocation_),
        .pfnReallocation = reinterpret_cast<PFN_vkReallocationFunction>(reallocation_),
        .pfnFree = reinterpret_cast<PFN_vkFreeFunction>(free_),
        .pfnInternalAllocation = reinterpret_cast<PFN_vkInternalAllocationNotification>(allocation_nortification_),
        .pfnInternalFree = reinterpret_cast<PFN_vkInternalFreeNotification>(free_notification_),
    } {}

    operator const VkAllocationCallbacks*() const noexcept { return &callbacks_; }
};

}