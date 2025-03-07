#pragma once

#include "common.hpp"
#include "Logger.hpp"

namespace graphics {

class HostMemoryAllocator {
    VkAllocationCallbacks callbacks_;
    struct AllocationRecord {
        size_t size;
        VkSystemAllocationScope scope;
    };
    std::unordered_map<void*, AllocationRecord> alloc_table_;
    // each scope
    std::array<size_t, 5> size_table_;
    // external / internal
    std::array<size_t, 2> size_table2_;

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
        if(p) {
            auto allocator = reinterpret_cast<HostMemoryAllocator*>(user_data);
            allocator->alloc_table_[p] = {size, scope};
            allocator->size_table_[scope] += size;
            allocator->size_table2_[0] += size;
        }
        return p;
    }

    static void* VKAPI_CALL reallocation_(void* user_data, void* original, size_t size, size_t alignment, VkSystemAllocationScope scope) {
        auto p = _aligned_realloc(original, size, alignment);
        if(p) {
            auto allocator = reinterpret_cast<HostMemoryAllocator*>(user_data);
            allocator->size_table_[allocator->alloc_table_[original].scope] -= allocator->alloc_table_[original].size;
            allocator->size_table2_[0] -= allocator->alloc_table_[original].size;
            allocator->alloc_table_.erase(original);
            allocator->alloc_table_[p] = {size, scope};
            allocator->size_table_[scope] += size;
            allocator->size_table2_[0] += size;
        }
        return p;
    }

    static void VKAPI_CALL free_(void* user_data, void* memory) {
        auto allocator = reinterpret_cast<HostMemoryAllocator*>(user_data);
        if(allocator->alloc_table_.find(memory) != allocator->alloc_table_.end()) {
            allocator->size_table_[allocator->alloc_table_[memory].scope] -= allocator->alloc_table_[memory].size;
            allocator->size_table2_[0] -= allocator->alloc_table_[memory].size;
            allocator->alloc_table_.erase(memory);
        }
        _aligned_free(memory);
    }

    static void VKAPI_CALL allocation_nortification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {
        auto allocator = reinterpret_cast<HostMemoryAllocator*>(user_data);
        allocator->size_table_[scope] += size;
        allocator->size_table2_[1] += size;
    }

    static void VKAPI_CALL free_notification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {
        auto allocator = reinterpret_cast<HostMemoryAllocator*>(user_data);
        allocator->size_table_[scope] -= size;
        allocator->size_table2_[1] -= size;
    }

    HostMemoryAllocator() noexcept : callbacks_ {
        .pUserData = this,
        .pfnAllocation = reinterpret_cast<PFN_vkAllocationFunction>(allocation_),
        .pfnReallocation = reinterpret_cast<PFN_vkReallocationFunction>(reallocation_),
        .pfnFree = reinterpret_cast<PFN_vkFreeFunction>(free_),
        .pfnInternalAllocation = reinterpret_cast<PFN_vkInternalAllocationNotification>(allocation_nortification_),
        .pfnInternalFree = reinterpret_cast<PFN_vkInternalFreeNotification>(free_notification_),
    }, alloc_table_{}, size_table_{}
    {
        LOG_INFO(std::format("host memory allcator is created."));
    }
    ~HostMemoryAllocator() noexcept {
        LOG_INFO(std::format("host memory allocator is destroyed.\nmemory leak checking..."));
        print_summary();
    }
    HostMemoryAllocator(const HostMemoryAllocator&) = delete;
    auto& operator=(const HostMemoryAllocator&) = delete;
    HostMemoryAllocator(HostMemoryAllocator&&) = delete;
    auto& operator=(HostMemoryAllocator&&) = delete;

public:
    static auto& get() noexcept {
        static HostMemoryAllocator instance{};
        return instance;
    }

    operator const VkAllocationCallbacks*() noexcept { return &callbacks_; }

    void print_summary() noexcept {
        std::string message{};
        message += "\n";
        for(size_t i = 0; i < size_table_.size(); ++i) {
            message += std::format("\tscope: {} -> {} used.\n", get_scope_str_(static_cast<VkSystemAllocationScope>(i)), byte_to_str(size_table_[i]));
        }
        message += std::format("\texternal allocation: {} / internal allocation: {}", byte_to_str(size_table2_[0]), byte_to_str(size_table2_[1]));
        LOG_INFO(message);
    }
};

}