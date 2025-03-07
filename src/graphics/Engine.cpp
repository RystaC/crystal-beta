#include "Engine.hpp"

namespace graphics {

std::unique_ptr<Engine> Engine::initialize() {
    auto engine = std::make_unique<Engine>();

    // create instance
    {
        std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        std::vector<const char*> extensions = {"VK_EXT_debug_utils"};
#else
        std::vector<const char*> extensions = {};
#endif

        VkApplicationInfo application_info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "test application",
            .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
            .pEngineName = ENGINE_NAME_,
            .engineVersion = ENGINE_VERSION_,
            .apiVersion = VK_API_VERSION_1_3,
        };
        VkInstanceCreateInfo instance_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = size_u32(layers),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = size_u32(extensions),
            .ppEnabledExtensionNames = extensions.data(),
        };

        auto result = vkw::Instance::create(instance_info, engine->instance_, HostMemoryAllocator::get());
    }

    // debug utils messenger (if we can use)
#if defined(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
    {
        engine->create_debug_utils_messenger_ = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(engine->instance_.get_proc_addr("vkCreateDebugUtilsMessengerEXT"));
        engine->destroy_debug_utils_messenger_ = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(engine->instance_.get_proc_addr("vkDestroyDebugUtilsMessengerEXT"));

        VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_utils_callback,
            .pUserData = nullptr,
        };

        auto result = engine->create_debug_utils_messenger_(*engine->instance_, &messenger_info, HostMemoryAllocator::get(), &engine->debug_utils_messenger_);
    }
#endif

    // physical device enumeration
    std::vector<VkPhysicalDevice> physical_devices{};
    {
        auto result = engine->instance_.enumerate_physical_devices(physical_devices);
    }
    auto physical_device = vkw::PhysicalDevice(physical_devices[0]);

    uint32_t graphics_queue_family_index = std::numeric_limits<uint32_t>::max();
    {
        std::vector<VkQueueFamilyProperties2> queue_family_properties{};
        physical_device.get_queue_family_properties(queue_family_properties);

        for(size_t i = 0; i < queue_family_properties.size(); ++i) {
            if((queue_family_properties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                graphics_queue_family_index = static_cast<uint32_t>(i);
            }
        }

        if(graphics_queue_family_index == std::numeric_limits<uint32_t>::max()) {
            LOG_ERROR(std::format("could not find graphic queue family index. exit."));
            std::exit(EXIT_FAILURE);
        }

        LOG_INFO(std::format("graphics queue family index = {}", graphics_queue_family_index));
    }

    VkPhysicalDeviceMemoryProperties2 memory_properties{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2};
    uint32_t device_local_exclusive_memory_index = U32_MAX;
    {
        physical_device.get_memory_properties(memory_properties);

        for(uint32_t i = 0; i < memory_properties.memoryProperties.memoryHeapCount; ++i) {
            LOG_INFO(std::format("#{}: size = {}, flags = {:08b}", i, memory_properties.memoryProperties.memoryHeaps[i].size, memory_properties.memoryProperties.memoryHeaps[i].flags));
        }

        for(uint32_t i = 0; i < memory_properties.memoryProperties.memoryTypeCount; ++i) {
            LOG_INFO(std::format("#{}: index = {}, flags = {:08b}", i, memory_properties.memoryProperties.memoryTypes[i].heapIndex, memory_properties.memoryProperties.memoryTypes[i].propertyFlags));
            if((memory_properties.memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) && !(memory_properties.memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
                device_local_exclusive_memory_index = i;
            } 
        }

        if(device_local_exclusive_memory_index == U32_MAX) {
            LOG_ERROR(std::format("could not find device local exclusive memory in your device. exit."));
            std::exit(EXIT_FAILURE);
        }

        LOG_INFO(std::format("device local exclusive memory index = {}", device_local_exclusive_memory_index));
    }

    // create device
    {
        std::vector<const char*> extensions = {};

        std::vector<float> queue_priorities = {0.0f};
        std::vector<VkDeviceQueueCreateInfo> queue_infos = {
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = graphics_queue_family_index,
                .queueCount = size_u32(queue_priorities),
                .pQueuePriorities = queue_priorities.data(),
            }
        };
        VkDeviceCreateInfo device_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = size_u32(queue_infos),
            .pQueueCreateInfos = queue_infos.data(),
            /* DEPRECATED */    .enabledLayerCount = 0,
            /* DEPRECATED */    .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = size_u32(extensions),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = nullptr,
        };

        auto result = vkw::Device::create(physical_device, device_info, engine->device_, HostMemoryAllocator::get());
    }

    // create device queue
    {
        VkDeviceQueueInfo2 queue_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = graphics_queue_family_index,
            .queueIndex = 0,
        };
        VkQueue queue{};
        engine->device_.get_queue(queue_info, queue);
        engine->queue_ = vkw::Queue(queue, 0, 0);
    }

    // test allocation of device memory
    VkDeviceMemory device_memory{};
    {
        VkMemoryAllocateInfo allocate_info = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = 1024,
            .memoryTypeIndex = device_local_exclusive_memory_index,
        };

        auto result = engine->device_.allocate_memory(allocate_info, device_memory);
    }

    engine->device_.free_memory(device_memory);

    return engine;
}

}