#pragma once

#include "common.hpp"

namespace vkw::handle {

template<typename T, void Destroyer(T, const VkAllocationCallbacks*)>
class PrimaryHandle {
    T entity_;
    const VkAllocationCallbacks* allocator_;

public:
    using entity_type = T;

    PrimaryHandle(T&& entity, const VkAllocationCallbacks* allocator = nullptr) noexcept : entity_(entiry), allocator_(allocator) {}
    ~PrimaryHandle() noexept {
        Destroyer(entity_, allocator_);
    }

    PrimaryHandle(const PrimaryHandle&) = delete;
    auto& operator=(const PrimaryHandle&) = delete;
    PrimaryHandle(PrimaryHandle&&) = default;
    auto& operator=(PrimaryHandle&&) = default;

    operator entity_type() const noexcept { return entity_; }
};

template<typename Parent, typename T, void Destroyer(Parent::entity_type, T, const VkAllocationCallbacks*)>
class Handle {
    std::shared_ptr<Parent> parent_ptr_;
    T entity_;
    const VkAllocationCallbacks* allocator_;

public:
    using entity_type = T;

    Handle(std::shared_ptr<Parent> parent_ptr, T&& entity, const VkAllocationCallbacks* allocator = nullptr) noexcept : parent_ptr_(parent_ptr), entity_(entity), allocator_(allocator) {}
    ~Handle() noexcept {
        if(parent_ptr_) {
            Destroyer(*parent_ptr_, entity_, allocator_);
        }
    }

    Handle(const Handle&) = delete;
    auto& operator=(const Handle&) = delete;
    Handle(Handle&&) = default;
    auto& operator=(Handle&&) = default;

    operator entity_type() const noexcept { return entity_; }
};


// primary objects

using Instance = PrimaryHandle<VkInstance, vkDestroyInstance>;
using Device = PrimaryHandle<VkDevice, vkDestroyDevice>;

// objects from primary object
// from instance

#if defined(VK_KHR_SURFACE_EXTENSION_NAME)
using Surface = Handle<Instance, VkSurfaceKHR, vkDestroySurfaceKHR>;
#endif
#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
using DebugUtilsMessenger = Handle<Instance, VkDebugUtilsMessengerEXT, vkDestroyDebugUtilsMessengerEXT>;
#endif

// from device
// except with free command (e.g. VkDeviceMemory with vkFreeDeviceMemory())
// VkCommandBuffer, VkDescriptorSet, VkDeviceMemory

using Buffer = Handle<Device, VkBuffer, vkDestroyBuffer>;
using BufferView = Handle<Device, VkBufferView, vkDestroyBufferView>;
using CommandPool = Handle<Device, VkCommandPool, vkDestroyCommandPool>;
using DescriptorPool = Handle<Device, VkDescriptorPool, vkDestroyDescriptorPool>;
using DescriptorSetLayout = Handle<Device, VkDescriptorSetLayout, vkDestroyDescriptorSetLayout>;
using DescriptorUpdateTemplate = Handle<Device, VkDescriptorUpdateTemplate, vkDestroyDescriptorUpdateTemplate>;
using Event = Handle<Device, VkEvent, vkDestroyEvent>;
using Fence = Handle<Device, VkFence, vkDestroyFence>;
using Framebuffer = Handle<Device, VkFramebuffer, vkDestroyFramebuffer>;
using Image = Handle<Device, VkImage, vkDestroyImage>;
using ImageView = Handle<Device, VkImageView, vkDestroyImageView>;
using Pipeline = Handle<Device, VkPipeline, vkDestroyPipeline>;
using PipelineCache = Handle<Device, VkPipelineCache, vkDestroyPipelineCache>;
using PipelineLayout = Handle<Device, VkPipelineLayout, vkDestroyPipelineLayout>;
using QueryPool = Handle<Device, VkQueryPool, vkDestroyQueryPool>;
using RenderPass = Handle<Device, VkRenderPass, vkDestroyRenderPass>;
using Sampler = Handle<Device, VkSampler, vkDestroySampler>;
using SamplerYcbcrConversion = Handle<Device, VkSamplerYcbcrConversion, vkDestroySamplerYcbcrConversion>;
using Semaphore = Handle<Device, VkSemaphore, vkDestroySemaphore>;
using ShaderModule = Handle<Device, VkShaderModule, vkDestroyShaderModule>;
#if defined(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
using Swapchain = Handle<Device, VkSwapchainKHR, vkDestroySwapchainKHR>;
#endif

}