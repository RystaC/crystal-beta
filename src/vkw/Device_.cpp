// #include "Device.hpp"

// namespace vkw {

// bool Device::init(PhysicalDevice&& physical_device, uint32_t queue_family_index, const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
//     // if(queue_targets.empty()) return false;

//     // queues_.resize(queue_targets.size());
//     // queue_family_indices_.resize(queue_targets.size());

//     // std::vector<VkDeviceQueueCreateInfo> queue_infos(queue_targets.size());
//     // for(size_t i = 0; i < queue_targets.size(); ++i) {
//     //     if(queue_targets[i].priorities.empty()) return false;

//     //     queues_[i].resize(queue_targets[i].priorities.size());

//     //     queue_infos[i] = {
//     //         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
//     //         .queueFamilyIndex = queue_targets[i].family_index,
//     //         .queueCount = size_u32(queue_targets[i].priorities.size()),
//     //         .pQueuePriorities = queue_targets[i].priorities.data(),
//     //     };

//     //     queue_family_indices_.push_back(queue_targets[i].family_index);
//     // }

//     physical_device_ = std::make_unique<PhysicalDevice>(physical_device);

//     queue_family_index_ = queue_family_index;

//     float priority = 0.0f;

//     VkDeviceQueueCreateInfo queue_info {
//         .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
//         .queueFamilyIndex = queue_family_index_,
//         .queueCount = 1u,
//         .pQueuePriorities = &priority,
//     };

//     VkDeviceCreateInfo device_info {
//         .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
//         .queueCreateInfoCount = 1u,
//         .pQueueCreateInfos = &queue_info,
//         .enabledLayerCount = size_u32(layers.size()),
//         .ppEnabledLayerNames = layers.data(),
//         .enabledExtensionCount = size_u32(extensions.size()),
//         .ppEnabledExtensionNames = extensions.data(),
//     };

//     VkDevice device{};
//     CHECK_VK_RESULT(vkCreateDevice(*physical_device_, &device_info, nullptr, &device), return false;);
    
//     device_ = std::make_shared<DeviceEntity>(std::move(device));

//     // for(size_t i = 0; i < queues_.size(); ++i) {
//     //     for(size_t j = 0; j < queues_[i].size(); ++j) {
//     //         vkGetDeviceQueue(*device_, queue_targets[i].family_index, size_u32(j), &queues_[i][j]);
//     //     }
//     // }

//     vkGetDeviceQueue(*device_, queue_family_index_, 0, &queue_);

//     return true;
// }

// std::unique_ptr<CommandPool> Device::create_command_pool() {
//     VkCommandPoolCreateInfo pool_info {
//         .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
//         .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
//         .queueFamilyIndex = queue_family_index_,
//     };

//     VkCommandPool command_pool{};
//     CHECK_VK_RESULT(vkCreateCommandPool(*device_, &pool_info, nullptr, &command_pool), return {};);

//     return std::make_unique<CommandPool>(device_, std::move(command_pool));
// }

// std::unique_ptr<Swapchain> Device::create_swapchain(const Surface& surface, const VkSurfaceFormatKHR& desired_format, const VkPresentModeKHR& desired_present_mode, uint32_t width, uint32_t height) {
//     VkBool32 is_supported{};
//     vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device_, queue_family_index_, surface, &is_supported);
//     if(!is_supported) return {};

//     VkSurfaceCapabilitiesKHR capabilities{};
//     vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physical_device_, surface, &capabilities);

//     uint32_t format_count{};
//     vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_, surface, &format_count, nullptr);
//     std::vector<VkSurfaceFormatKHR> formats(format_count);
//     vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device_, surface, &format_count, formats.data());

//     uint32_t present_mode_count{};
//     vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_, surface, &present_mode_count, nullptr);
//     std::vector<VkPresentModeKHR> present_modes(present_mode_count);
//     vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device_, surface, &present_mode_count, present_modes.data());

//     VkSwapchainCreateInfoKHR swapchain_info {
//         .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
//         .surface = surface,
//         .minImageCount = 2u,
//         .imageFormat = desired_format.format,
//         .imageColorSpace = desired_format.colorSpace,
//         .imageExtent = { width, height },
//         .imageArrayLayers = 1u,
//         .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
//         .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
//         .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
//         .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
//         .presentMode = desired_present_mode,
//         .clipped = VK_TRUE,
//     };

//     VkSwapchainKHR swapchain{};
//     CHECK_VK_RESULT(vkCreateSwapchainKHR(*device_, &swapchain_info, nullptr, &swapchain), return {};);

//     uint32_t image_count{};
//     CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, nullptr), return {};);
//     std::vector<VkImage> images(image_count);
//     CHECK_VK_RESULT(vkGetSwapchainImagesKHR(*device_, swapchain, &image_count, images.data()), return {};);

//     std::vector<VkImageView> image_views(image_count);
//     for(size_t i = 0; i < images.size(); ++i) {
//         VkImageViewCreateInfo view_info {
//             .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//             .image = images[i],
//             .viewType = VK_IMAGE_VIEW_TYPE_2D,
//             .format = desired_format.format,
//             .components = {
//                 .r = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .g = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .b = VK_COMPONENT_SWIZZLE_IDENTITY,
//                 .a = VK_COMPONENT_SWIZZLE_IDENTITY,
//             },
//             .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
//         };

//         CHECK_VK_RESULT(vkCreateImageView(*device_, &view_info, nullptr, &image_views[i]), return {};);
//     }

//     return std::make_unique<Swapchain>(device_, std::move(swapchain), std::move(images), std::move(image_views), width, height);
// }

// std::unique_ptr<RenderPass> Device::create_render_pass(const AttachmentDescriptions& attachment_descs, const SubpassDescriptions subpass_descs, const SubpassDependencies subpass_dependencies) {
//     VkRenderPassCreateInfo render_pass_info {
//         .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
//         .attachmentCount = size_u32(attachment_descs.descs_.size()),
//         .pAttachments = attachment_descs.descs_.data(),
//         .subpassCount = size_u32(subpass_descs.subpasses_.size()),
//         .pSubpasses = subpass_descs.subpasses_.data(),
//         .dependencyCount = size_u32(subpass_dependencies.dependencies_.size()),
//         .pDependencies = subpass_dependencies.dependencies_.data(),
//     };

//     VkRenderPass render_pass{};
//     CHECK_VK_RESULT(vkCreateRenderPass(*device_, &render_pass_info, nullptr, &render_pass), return {};);

//     return std::make_unique<RenderPass>(device_, std::move(render_pass));
// }

// std::unique_ptr<Framebuffer> Device::create_framebuffer(const RenderPass& render_pass, const std::vector<VkImageView>& image_views, uint32_t width, uint32_t height) {
//     VkFramebufferCreateInfo framebuffer_info {
//         .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
//         .renderPass = render_pass,
//         .attachmentCount = size_u32(image_views.size()),
//         .pAttachments = image_views.data(),
//         .width = width,
//         .height = height,
//         .layers = 1u,
//     };

//     VkFramebuffer framebuffer{};
//     CHECK_VK_RESULT(vkCreateFramebuffer(*device_, &framebuffer_info, nullptr, &framebuffer), return {};);

//     return std::make_unique<Framebuffer>(device_, std::move(framebuffer));
// }

// std::unique_ptr<ShaderModule> Device::create_shader_module(const std::filesystem::path& spirv_path) {
//     std::ifstream ifs(spirv_path, std::ios::binary | std::ios::ate);
//     if(ifs.fail()) {
//         std::cerr << "[vkw::Device::create_shader_module] ERROR: could not read file: " << spirv_path << std::endl;
//         return {};
//     }

//     auto bin_size = static_cast<size_t>(ifs.tellg());
//     ifs.seekg(0, std::ios::beg);

//     std::vector<char> bin_data(bin_size);
//     ifs.read(bin_data.data(), bin_size);
//     ifs.close();

//     VkShaderModuleCreateInfo module_info {
//         .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
//         .codeSize = bin_size,
//         .pCode = reinterpret_cast<uint32_t*>(bin_data.data()),
//     };

//     VkShaderModule shader_module{};
//     CHECK_VK_RESULT(vkCreateShaderModule(*device_, &module_info, nullptr, &shader_module), return {};);

//     return std::make_unique<ShaderModule>(device_, std::move(shader_module));
// }

// std::unique_ptr<DescriptorSetLayout> Device::create_descriptor_layout(const std::vector<VkDescriptorSetLayoutBinding>& layout_bindings) {
//     VkDescriptorSetLayoutCreateInfo layout_info {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
//         .bindingCount = size_u32(layout_bindings.size()),
//         .pBindings = layout_bindings.data(),
//     };

//     VkDescriptorSetLayout descriptor_layout{};
//     CHECK_VK_RESULT(vkCreateDescriptorSetLayout(*device_, &layout_info, nullptr, &descriptor_layout), return {};);

//     return std::make_unique<DescriptorSetLayout>(device_, std::move(descriptor_layout));
// }

// std::unique_ptr<DescriptorPool> Device::create_descriptor_pool(const std::vector<VkDescriptorPoolSize>& pool_sizes, uint32_t max_sets) {
//     VkDescriptorPoolCreateInfo pool_info {
//         .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
//         .maxSets = max_sets,
//         .poolSizeCount = size_u32(pool_sizes.size()),
//         .pPoolSizes = pool_sizes.data(),
//     };

//     VkDescriptorPool descriptor_pool{};
//     CHECK_VK_RESULT(vkCreateDescriptorPool(*device_, &pool_info, nullptr, &descriptor_pool), return {};);

//     return std::make_unique<DescriptorPool>(device_, std::move(descriptor_pool));
// }

// std::unique_ptr<Pipeline> Device::create_graphics_pipeline(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const std::vector<VkPushConstantRange>& push_constant_ranges, const GraphicsPipelineStates& pipeline_states, const RenderPass& render_pass, uint32_t subpass_index) {
//     VkPipelineLayoutCreateInfo layout_info {
//         .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
//         .setLayoutCount = size_u32(descriptor_set_layouts.size()),
//         .pSetLayouts = descriptor_set_layouts.data(),
//         .pushConstantRangeCount = size_u32(push_constant_ranges.size()),
//         .pPushConstantRanges = push_constant_ranges.data(),
//     };

//     VkPipelineLayout pipeline_layout{};
//     CHECK_VK_RESULT(vkCreatePipelineLayout(*device_, &layout_info, nullptr, &pipeline_layout), return {};);

//     VkGraphicsPipelineCreateInfo pipeline_info {
//         .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
//         .stageCount = size_u32(pipeline_states.shader_stages_.size()),
//         .pStages = pipeline_states.shader_stages_.data(),
//         .pVertexInputState = &pipeline_states.vertex_input_state_,
//         .pInputAssemblyState = &pipeline_states.input_assembly_state_,
//         .pViewportState = &pipeline_states.viewport_state_,
//         .pRasterizationState = &pipeline_states.rasterization_state_,
//         .pMultisampleState = &pipeline_states.multisample_state_,
//         .pDepthStencilState = &pipeline_states.depth_stencil_state_,
//         .pColorBlendState = &pipeline_states.color_blend_state_,
//         .pDynamicState = &pipeline_states.dynamic_state_,
//         .layout = pipeline_layout,
//         .renderPass = render_pass,
//         .subpass = subpass_index,
//     };

//     VkPipeline pipeline{};

//     CHECK_VK_RESULT(vkCreateGraphicsPipelines(*device_, VK_NULL_HANDLE, 1u, &pipeline_info, nullptr, &pipeline), return {};);

//     return std::make_unique<Pipeline>(device_, std::move(pipeline), std::move(pipeline_layout));
// }

// VkDeviceMemory Device::allocate_device_memory(const VkMemoryRequirements& requirements) {
//     uint32_t memory_index = UINT32_MAX;
//     auto& memory_properties = physical_device_->memory_properties_.memoryTypes;
//     for(uint32_t i = 0u; i < physical_device_->memory_properties_.memoryTypeCount; ++i) {
//         if((memory_properties[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
//             memory_index = i;
//             break;
//         }
//     }
//     if(memory_index == UINT32_MAX) return VK_NULL_HANDLE;

//     VkMemoryAllocateInfo allocate_info {
//         .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
//         .allocationSize = requirements.size,
//         .memoryTypeIndex = memory_index,
//     };

//     VkDeviceMemory device_memory{};
//     CHECK_VK_RESULT(vkAllocateMemory(*device_, &allocate_info, nullptr, &device_memory), return VK_NULL_HANDLE;);

//     return device_memory;
// }

// std::unique_ptr<Fence> Device::create_fence() {
//     VkFenceCreateInfo fence_info {
//         .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
//     };

//     VkFence fence{};
//     CHECK_VK_RESULT(vkCreateFence(*device_, &fence_info, nullptr, &fence), return {};);

//     return std::make_unique<Fence>(device_, std::move(fence));
// }

// void Device::submit_commands(CommandBuffer& command_buffer, Fence& fence) {
//     VkCommandBuffer cb = command_buffer;
//     VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

//     VkSubmitInfo submit_info {
//         .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
//         .waitSemaphoreCount = 0u,
//         .pWaitSemaphores = nullptr,
//         .pWaitDstStageMask = &wait_stage_mask,
//         .commandBufferCount = 1u,
//         .pCommandBuffers = &cb,
//         .signalSemaphoreCount = 0u,
//         .pSignalSemaphores = nullptr,
//     };

//     CHECK_VK_RESULT(vkQueueSubmit(queue_, 1u, &submit_info, fence), return;);
// }

// void Device::present(Swapchain& swapchain, uint32_t index) {
//     VkSwapchainKHR sc = swapchain;
//     VkPresentInfoKHR present_info {
//         .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
//         .waitSemaphoreCount = 0u,
//         .pWaitSemaphores = nullptr,
//         .swapchainCount = 1u,
//         .pSwapchains = &sc,
//         .pImageIndices = &index,
//         .pResults = nullptr,
//     };

//     CHECK_VK_RESULT(vkQueuePresentKHR(queue_, &present_info), return;);
// }

// std::unique_ptr<Image> Device::create_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage) {
//     VkImageCreateInfo image_info {
//         .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
//         .imageType = VK_IMAGE_TYPE_2D,
//         .format = format,
//         .extent = { .width = width, .height = height, .depth = 1u },
//         .mipLevels = 1u,
//         .arrayLayers = 1u,
//         .samples = VK_SAMPLE_COUNT_1_BIT,
//         .tiling = tiling,
//         .usage = usage,
//         .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
//         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//     };

//     VkImage image{};
//     CHECK_VK_RESULT(vkCreateImage(*device_, &image_info, nullptr, &image), return {};);

//     auto memory = allocate_memory(image);
//     CHECK_VK_RESULT(vkBindImageMemory(*device_, image, memory, 0), return {};);

//     return std::make_unique<Image>(device_, std::move(image), std::move(memory), format);
// }

// }