#include "Device.hpp"

namespace vkw {

Result<Device> Device::init(const object::PhysicalDevice& physical_device, const queue::CreateInfos& queue_infos, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
    auto device = Device(physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_infos.infos_.size());
    auto j = queue_infos.infos_.begin();
    for(size_t i = 0; i < queue_create_infos.size(); ++i) {
        queue_create_infos[i] = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = j->first,
            .queueCount = size_u32(j->second.size()),
            .pQueuePriorities = j->second.data(),
        };

        device.queue_counts_[queue_create_infos[i].queueFamilyIndex] += queue_create_infos[i].queueCount;
        ++j;
    }

    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = size_u32(queue_create_infos.size()),
        .pQueueCreateInfos = queue_create_infos.data(),
        .enabledLayerCount = size_u32(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = size_u32(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkDevice device_entity{};
    auto result = vkCreateDevice(device.physical_device_, &device_info, nullptr, &device_entity);

    device.device_ = std::make_shared<object::Device>(std::move(device_entity));

    return Result(std::move(device), result);
}

std::vector<object::Queue> Device::create_queues(uint32_t queue_family_index) {
    std::vector<VkQueue> raw_queues(queue_counts_[queue_family_index]);
    for(uint32_t i = 0; i < queue_counts_[queue_family_index]; ++i) {
        vkGetDeviceQueue(*device_, queue_family_index, i, &raw_queues[i]);
    }

    std::vector<object::Queue> queues(queue_counts_[queue_family_index]);
    std::transform(raw_queues.begin(), raw_queues.end(), queues.begin(), [&](auto& q) { return object::Queue(std::move(q), queue_family_index); });

    return queues;
}

Result<object::Swapchain> Device::create_swapchain(const object::Surface& surface, uint32_t queue_family_index, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, const VkExtent2D& extent) {
    auto surface_capabilities = physical_device_.surface_capabilities_(surface, queue_family_index);

    // TODO: check surface compatibilities for swapchain

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = 2,
        .imageFormat = desired_format.format,
        .imageColorSpace = desired_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = desired_present_mode,
        .clipped = VK_TRUE,
    };

    VkSwapchainKHR swapchain{};
    VkResult result{};
    result = vkCreateSwapchainKHR(*device_, &swapchain_info, nullptr, &swapchain);

    uint32_t image_count{};
    result = vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, nullptr);
    std::vector<VkImage> images(image_count);
    result = vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, images.data());

    std::vector<VkImageView> image_views(image_count);
    for(size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo view_info {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = desired_format.format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        result = vkCreateImageView(*device_, &view_info, nullptr, &image_views[i]);
    }    

    return Result(object::Swapchain(device_, std::move(swapchain), std::move(images), std::move(image_views), desired_format.format, extent), result);
}

Result<object::DeviceMemory> Device::allocate_memory(VkDeviceSize size, uint32_t memory_type_index) {
    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = size,
        .memoryTypeIndex = memory_type_index,
    };

    VkDeviceMemory memory{};
    auto result = vkAllocateMemory(*device_, &allocate_info, nullptr, &memory);

    return Result(object::DeviceMemory(device_, std::move(memory)), result);
}

VkDeviceMemory Device::allocate_memory_with_requirements_(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags desired_properties) {
    uint32_t memory_type_index = std::numeric_limits<uint32_t>::max();
    auto& memory_properties = physical_device_.memory_properties_;
    uint32_t type_bits = 1;
    for(uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if((requirements.memoryTypeBits & type_bits) != 0) {
            if((memory_properties.memoryTypes[i].propertyFlags & desired_properties) != 0) {
                memory_type_index = i;
                break;
            }
        }
    }
    if(memory_type_index == std::numeric_limits<uint32_t>::max()) {
        std::cerr << "[vkw::Device::allocate_memory_with_requirements_] ERROR: cannot find memory type with resource requirements and desired properties." << std::endl;
        return VK_NULL_HANDLE;
    }

    VkMemoryAllocateInfo allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memory_type_index,
    };

    VkDeviceMemory memory{};
    vkAllocateMemory(*device_, &allocate_info, nullptr, &memory);

    return memory;
}

Result<object::Image> Device::create_image(const VkExtent2D& extent_2d, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags desired_properties) {
    VkImageCreateInfo image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = extent_2d.width,
            .height = extent_2d.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImage image{};
    VkResult result{};
    result = vkCreateImage(*device_, &image_info, nullptr, &image);

    auto requirements = query_memory_requirements_(image);
    auto device_memory = allocate_memory_with_requirements_(requirements, desired_properties);

    result = vkBindImageMemory(*device_, image, device_memory, 0);

    return Result(object::Image(device_, std::move(image), std::move(device_memory), format), result);
}

Result<object::Sampler> Device::create_sampler(VkFilter min_filter, VkFilter mag_filter, VkSamplerAddressMode address_mode_u, VkSamplerAddressMode address_mode_v) {
    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = mag_filter,
        .minFilter = min_filter,
        .addressModeU = address_mode_u,
        .addressModeV = address_mode_v,
    };

    VkSampler sampler{};
    auto result = vkCreateSampler(*device_, &sampler_info, nullptr, &sampler);

    return Result(object::Sampler(device_, std::move(sampler)), result);
}

Result<object::RenderPass> Device::create_render_pass(const render_pass::AttachmentDescriptions& attachment_descriptions, const render_pass::SubpassDescriptions& subpass_descriptions, const std::optional<render_pass::SubpassDependencies>& subpass_dependencies) {
    VkRenderPassCreateInfo render_pass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = size_u32(attachment_descriptions.descs_.size()),
        .pAttachments = attachment_descriptions.descs_.data(),
        .subpassCount = size_u32(subpass_descriptions.subpasses_.size()),
        .pSubpasses = subpass_descriptions.subpasses_.data(),
        .dependencyCount = subpass_dependencies.has_value() ? size_u32(subpass_dependencies.value().dependencies_.size()) : 0,
        .pDependencies = subpass_dependencies.has_value() ? subpass_dependencies.value().dependencies_.data() : nullptr,
    };

    VkRenderPass render_pass{};
    auto result = vkCreateRenderPass(*device_, &render_pass_info, nullptr, &render_pass);

    return Result(object::RenderPass(device_, std::move(render_pass)), result);
}

Result<object::Framebuffer> Device::create_framebuffer(const object::RenderPass& render_pass, const std::vector<VkImageView>& attachments, const VkExtent2D& extent) {
    VkFramebufferCreateInfo framebuffer_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = render_pass,
        .attachmentCount = size_u32(attachments.size()),
        .pAttachments = attachments.data(),
        .width = extent.width,
        .height = extent.height,
        .layers = 1,
    };

    VkFramebuffer framebuffer{};
    auto result = vkCreateFramebuffer(*device_, &framebuffer_info, nullptr, &framebuffer);

    return Result(object::Framebuffer(device_, std::move(framebuffer)), result);
}

Result<object::ShaderModule> Device::create_shader_module(const std::filesystem::path& spirv_path) {
    std::ifstream ifs(spirv_path, std::ios::binary | std::ios::ate);
    if(ifs.fail()) {
        std::cerr << "[vkw::Device::create_shader_module] ERROR: cound not read file: " << spirv_path << std::endl;
    }

    auto binary_size = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    std::vector<char> binary_data(binary_size);
    ifs.read(binary_data.data(), binary_size);
    ifs.close();

    VkShaderModuleCreateInfo module_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = binary_size,
        .pCode = reinterpret_cast<uint32_t*>(binary_data.data()),
    };

    VkShaderModule shader_module{};
    auto result = vkCreateShaderModule(*device_, &module_info, nullptr, &shader_module);

    return Result(object::ShaderModule(device_, std::move(shader_module)), result);
}

Result<object::DescriptorSetLayout> Device::create_descriptor_set_layout(const descriptor::DescriptorSetLayoutBindings& layout_bindings) {
    VkDescriptorSetLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = size_u32(layout_bindings.bindings_.size()),
        .pBindings = layout_bindings.bindings_.data(),
    };

    VkDescriptorSetLayout layout{};
    auto result = vkCreateDescriptorSetLayout(*device_, &layout_info, nullptr, &layout);

    return Result(object::DescriptorSetLayout(device_, std::move(layout), layout_bindings.type_), result);
}

Result<object::DescriptorPool> Device::create_descriptor_pool(const std::vector<descriptor::DescriptorSetLayoutBindings>& layouts_for_pool) {
    std::vector<VkDescriptorPoolSize> pool_sizes(layouts_for_pool.size());
    for(size_t i = 0; i < pool_sizes.size(); ++i) {
        pool_sizes[i] = {
            .type = layouts_for_pool[i].type_,
            .descriptorCount = size_u32(layouts_for_pool[i].bindings_.size()),
        };
    }

    VkDescriptorPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = size_u32(pool_sizes.size()),
        .poolSizeCount = size_u32(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    };

    VkDescriptorPool descriptor_pool{};
    auto result = vkCreateDescriptorPool(*device_, &pool_info, nullptr, &descriptor_pool);

    return Result(object::DescriptorPool(device_, std::move(descriptor_pool)), result);
}

void Device::update_descriptor_sets(const descriptor::WriteDescriptorSets& write_descriptor_sets) {
    vkUpdateDescriptorSets(*device_, size_u32(write_descriptor_sets.writes_.size()), write_descriptor_sets.writes_.data(), 0, nullptr);
}

Result<object::PipelineLayout> Device::create_pipeline_layout(const pipeline_layout::CreateInfo& info) {
    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = size_u32(info.layouts_.size()),
        .pSetLayouts = info.layouts_.empty() ? nullptr : info.layouts_.data(),
        .pushConstantRangeCount = size_u32(info.ranges_.size()),
        .pPushConstantRanges = info.ranges_.empty() ? nullptr : info.ranges_.data(),
    };

    VkPipelineLayout layout{};
    auto result = vkCreatePipelineLayout(*device_, &layout_info, nullptr, &layout);

    return Result(object::PipelineLayout(device_, std::move(layout)), result);
}

Result<object::PipelineCache> Device::create_pipeline_cache() {
    VkPipelineCacheCreateInfo cache_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .initialDataSize = 0,
        .pInitialData = nullptr,
    };

    VkPipelineCache cache{};
    auto result = vkCreatePipelineCache(*device_, &cache_info, nullptr, &cache);

    return Result(object::PipelineCache(device_, std::move(cache)), result);
}

Result<object::PipelineCache> Device::create_pipeline_cache(const std::filesystem::path& cache_path) {
    std::ifstream ifs(cache_path, std::ios::binary | std::ios::ate);
    if(ifs.fail()) {
        std::cerr << "[vkw::Device::create_pipeline_cache] ERROR: cound not read file: " << cache_path << std::endl;
    }

    auto binary_size = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    std::vector<char> binary_data(binary_size);
    ifs.read(binary_data.data(), binary_size);
    ifs.close();

    VkPipelineCacheCreateInfo cache_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .initialDataSize = binary_size,
        .pInitialData = reinterpret_cast<void*>(binary_data.data()),
    };

    VkPipelineCache cache{};
    auto result = vkCreatePipelineCache(*device_, &cache_info, nullptr, &cache);

    return Result(object::PipelineCache(device_, std::move(cache)), result);
}

Result<object::Pipeline> Device::create_pipeline(const pipeline::GraphicsPipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkRenderPass& render_pass, uint32_t subpass_index, const VkPipelineCache& cache) {
    VkGraphicsPipelineCreateInfo pipeline_info = pipeline_states.pipeline_info_;
    pipeline_info.layout = pipeline_layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = subpass_index;

    VkPipeline pl{};
    auto result = vkCreateGraphicsPipelines(*device_, cache, 1, &pipeline_info, nullptr, &pl);

    return Result(object::Pipeline(device_, std::move(pl)), result);
}

Result<object::Pipeline> Device::create_pipeline(const pipeline::ComputePipelineStates& pipeline_states, const VkPipelineLayout pipeline_layout, const VkPipelineCache& cache) {
    VkComputePipelineCreateInfo pipeline_info = pipeline_states.pipeline_info_;
    pipeline_info.layout = pipeline_layout;

    VkPipeline pl{};
    auto result = vkCreateComputePipelines(*device_, cache, 1, &pipeline_info, nullptr, &pl);

    return Result(object::Pipeline(device_, std::move(pl)), result);
}

Result<object::CommandPool> Device::create_command_pool(VkCommandPoolCreateFlags flags, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = flags,
        .queueFamilyIndex = queue_family_index,
    };

    VkCommandPool pool{};
    auto result = vkCreateCommandPool(*device_, &pool_info, nullptr, &pool);

    return Result(object::CommandPool(device_, std::move(pool)), result);
}

Result<object::Event> Device::create_event() {
    VkEventCreateInfo event_info = {
        .sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
    };

    VkEvent event{};
    auto result = vkCreateEvent(*device_, &event_info, nullptr, &event);

    return Result(object::Event(device_, std::move(event)), result);
}

Result<object::Fence> Device::create_fence(bool signaled) {
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : static_cast<VkFenceCreateFlags>(0),
    };

    VkFence fence{};
    auto result = vkCreateFence(*device_, &fence_info, nullptr, &fence);

    return Result(object::Fence(device_, std::move(fence)), result);
}

Result<object::Semaphore> Device::create_semaphore() {
    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphore{};
    auto result = vkCreateSemaphore(*device_, &semaphore_info, nullptr, &semaphore);

    return Result(object::Semaphore(device_, std::move(semaphore)), result);
}

}