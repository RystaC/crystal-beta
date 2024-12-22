#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <cmath>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Game.hpp"

#include "vkw/vkw.hpp"

#include "font/TrueType.hpp"

#include "image/PNG.hpp"

constexpr size_t WINDOW_WIDTH = 512;
constexpr size_t WINDOW_HEIGHT = 512;

struct ForwardConstantData {
    glm::mat4 model, view, projection;
};

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

int main(int argc, char** argv) {
    image::PNG png{};
    try {
        png = std::move(image::PNG::load("../asset/texture/chuunibyou_girl.png"));
    }
    catch(std::exception& e) {
        std::cerr << std::format("exception occured. {}", e.what());
        std::exit(EXIT_FAILURE);
    }

    return 0;

    font::TrueType ttf{};
    try {
        ttf = std::move(font::TrueType::load("../asset/font/ipag.ttf"));
    }
    catch(std::exception& e) {
        std::cerr << std::format("exception occured. {}", e.what());
        std::exit(EXIT_FAILURE);
    }

    auto [vertices, indices] = ttf.glyphs(U"監督");
    // const char32_t* str = U"編";
    // auto [vertices, indices] = ttf.glyph_tess(str[0]);
    // auto points = ttf.point_vertices(str[0]);

    auto game = std::make_unique<Game>();
    auto result = game->init(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!result) {
        std::cerr << "[crystal-beta] ERROR: failed to initialize application. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto version = vkw::Instance::version();
    std::cerr << "Vulkan version: " << VK_API_VERSION_MAJOR(version) << "." << VK_API_VERSION_MINOR(version) << "." << VK_API_VERSION_PATCH(version) << ", variant = " << VK_API_VERSION_VARIANT(version) << std::endl;
    
    // std::cerr << std::endl << "enumerate instance layers..." << std::endl;
    const auto instance_layers = vkw::Instance::enumerate_layers();
    for(const auto& layer : instance_layers) {
        // std::cerr << layer.layerName << std::endl;
    }

    // std::cerr << std::endl << "enumerate default instance extensions..." << std::endl;
    const auto instance_extensions = vkw::Instance::enumerate_extensions();
    bool enable_debug_utils = false;
    for(const auto& extension : instance_extensions) {
        // std::cerr << extension.extensionName << std::endl;
        if(std::string(extension.extensionName) == "VK_EXT_debug_utils") {
            enable_debug_utils = true;
        }
    }

    // if VK_EXT_debug_utils is not supported -> exit
    if(!enable_debug_utils) {
        std::cerr << "[crystal-beta] ERROR: VK_EXT_debug_utils is not supported. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for(const auto& layer : instance_layers) {
        // std::cerr << std::endl << "enumerate instance extensions for " << layer.layerName << "..." << std::endl;
        const auto layer_extensions = vkw::Instance::enumerate_extensions(layer.layerName);
        for(const auto& extension : layer_extensions) {
            // std::cerr << extension.extensionName << std::endl;
        }
    }

    std::cerr << std::endl;

    // enum least required extensions
    auto desired_instance_extensions = game->enum_instance_extensions();
    desired_instance_extensions.push_back("VK_EXT_debug_utils");
    // activate validation layer
    auto desired_instance_layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
    auto instance = std::make_unique<vkw::Instance>(vkw::Instance::init(desired_instance_extensions, desired_instance_layers, nullptr).unwrap());

    auto surface = instance->create_surface_SDL(game->window()).unwrap();

    auto debug_utils_messenger = instance->create_debug_utils_messenger(
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        debug_utils_callback, 
        nullptr
    ).unwrap();

    std::cerr << std::endl << "enumerate physical devices..." << std::endl;
    auto physical_devices = instance->enum_physical_devices();
    for(const auto& pd: physical_devices) {
        std::cerr << pd.name() << std::endl;
    }

    std::cerr << std::endl << "physical device 0 will be used." << std::endl;

    // std::cerr << std::endl << "enumerate device layers..." << std::endl;
    const auto device_layers = vkw::Device::enumerate_layers(physical_devices[0]);
    for(const auto& layer : device_layers) {
        // std::cerr << layer.layerName << std::endl;
    }

    // std::cerr << std::endl << "enumerate default device extensions..." << std::endl;
    const auto device_extensions = vkw::Device::enumerate_extensions(physical_devices[0]);
    for(const auto& extension : device_extensions) {
        // std::cerr << extension.extensionName << std::endl;
    }

    for(const auto& layer : device_layers) {
        // std::cerr << std::endl << "enumerate device extensions for " << layer.layerName << "..." << std::endl;
        const auto layer_extensions = vkw::Device::enumerate_extensions(physical_devices[0], layer.layerName);
        for(const auto& extension : layer_extensions) {
            // std::cerr << extension.extensionName << std::endl;
        }
    }

    std::cerr << std::endl;

    auto queue_family_index = physical_devices[0].find_queue_family(VK_QUEUE_GRAPHICS_BIT);

    vkw::queue::CreateInfos queue_create_infos{};
    queue_create_infos.add(queue_family_index, {0.0f});

    std::vector<const char*> desired_device_extensions = { "VK_KHR_swapchain" };
    std::vector<const char*> desired_device_layers = { "VK_LAYER_KHRONOS_validation" };

    VkPhysicalDeviceFeatures device_features {
        .geometryShader = VK_TRUE,
        .tessellationShader = VK_TRUE,
        .fillModeNonSolid = VK_TRUE,
        .wideLines = VK_TRUE,
    };
    auto device = std::make_unique<vkw::Device>(vkw::Device::init(physical_devices[0], device_features, queue_create_infos, desired_device_extensions, desired_device_layers, nullptr).unwrap());

    std::cerr << std::endl << "create device queue for graphics..." << std::endl;
    auto main_queues = device->create_queues(queue_family_index);
    std::cerr << "queue family index = " << main_queues[0].family_index() << ", size = " << main_queues.size() << std::endl;

    std::cerr << std::endl << "create swapchain..." << std::endl;
    auto swapchain = device->create_swapchain(surface, queue_family_index, {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, VK_PRESENT_MODE_FIFO_KHR, {WINDOW_WIDTH, WINDOW_HEIGHT}).unwrap();

    std::cerr << std::endl << "create render pass..." << std::endl;
    // line render pass
    vkw::render_pass::AttachmentDescriptions line_attachment{};
    line_attachment
    // attachment 0: swapchain image
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    );

    vkw::render_pass::AttachmentReferences line_attachment_ref{};
    line_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions line_subpass{};
    line_subpass.add()
    .output_color_attachments(line_attachment_ref)
    .end();

    auto line_render_pass = device->create_render_pass(line_attachment, line_subpass).unwrap();

    // // point render pass
    // vkw::render_pass::AttachmentDescriptions point_attachment{};
    // point_attachment
    // // attachment 0: swapchain image
    // .add(
    //     VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
    //     VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
    //     VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
    //     {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    // );

    // vkw::render_pass::AttachmentReferences point_attachment_ref{};
    // point_attachment_ref
    // .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // vkw::render_pass::SubpassDescriptions point_subpass{};
    // point_subpass.add()
    // .output_color_attachments(point_attachment_ref)
    // .end();

    // auto point_render_pass = device->create_render_pass(point_attachment, point_subpass).unwrap();

    std::cerr << std::endl << "create framebuffers..." << std::endl;
    std::vector<vkw::resource::Framebuffer> line_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        line_framebuffers[i] = device->create_framebuffer(line_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    // std::vector<vkw::resource::Framebuffer> point_framebuffers(swapchain.size());
    // for(size_t i = 0; i < swapchain.size(); ++i) {
    //     point_framebuffers[i] = device->create_framebuffer(point_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    // }

    std::cerr << std::endl << "create shader modules..." << std::endl;
    auto line_vertex_shader = device->create_shader_module("spirv/ttf_line.vert.glsl.spirv").unwrap();
    auto line_tess_cont_shader = device->create_shader_module("spirv/ttf_line.tesc.glsl.spirv").unwrap();
    auto line_tess_eval_shader = device->create_shader_module("spirv/ttf_line.tese.glsl.spirv").unwrap();
    auto line_fragment_shader = device->create_shader_module("spirv/ttf_line.frag.glsl.spirv").unwrap();

    // auto point_vertex_shader = device->create_shader_module("spirv/ttf_point.vert.glsl.spirv").unwrap();
    // auto point_fragment_shader = device->create_shader_module("spirv/ttf_point.frag.glsl.spirv").unwrap();

    std::cerr << std::endl << "create buffers..." << std::endl;
    auto vertex_buffer = device->create_buffer_with_data(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto index_buffer = device->create_buffer_with_data(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();

    // auto point_buffer = device->create_buffer_with_data(points, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();

    std::cerr << std::endl << "create pipeline layout..." << std::endl;
    vkw::pipeline_layout::CreateInfo line_layout_info{};
    auto line_pipeline_layout = device->create_pipeline_layout(line_layout_info).unwrap();

    // vkw::pipeline_layout::CreateInfo point_layout_info{};
    // auto point_pipeline_layout = device->create_pipeline_layout(point_layout_info).unwrap();

    std::cerr << std::endl << "create pipelines..." << std::endl;

    vkw::pipeline::GraphicsShaderStages line_shader_stages{};
    line_shader_stages
    .vertex_shader(line_vertex_shader)
    .tessellation_control_shader(line_tess_cont_shader)
    .tessellation_evaluation_shader(line_tess_eval_shader)
    .fragment_shader(line_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions line_input_bindings{};
    line_input_bindings
    .add(0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX);
    vkw::pipeline::VertexInputAttributeDescriptions line_input_attributes{};
    line_input_attributes
    .add(0, 0, VK_FORMAT_R32G32_SFLOAT, 0);
    vkw::pipeline::VertexInputState line_vertex_input_state(line_input_bindings, line_input_attributes);
    vkw::pipeline::InputAssemblyState line_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
    vkw::pipeline::TessellationState line_tessellation_state(3);
    vkw::pipeline::ViewportState line_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState line_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState line_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState line_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates line_blend_attachment_states{};
    line_blend_attachment_states
    .add();
    vkw::pipeline::ColorBlendState line_color_blend_state(line_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates line_pipeline_state{};
    line_pipeline_state
    .shader_stages(line_shader_stages)
    .vertex_input(line_vertex_input_state)
    .input_assembly(line_input_assembly_state)
    .tessellation(line_tessellation_state)
    .viewport(line_viewport_state)
    .rasterization(line_rasterization_state)
    .multisample(line_multisample_state)
    .depth_stencil(line_depth_stencil_state)
    .color_blend(line_color_blend_state);

    auto line_pipeline = device->create_pipeline(line_pipeline_state, line_pipeline_layout, line_render_pass, 0).unwrap();

    // vkw::pipeline::GraphicsShaderStages point_shader_stages{};
    // point_shader_stages
    // .vertex_shader(point_vertex_shader)
    // .fragment_shader(point_fragment_shader);
    // vkw::pipeline::VertexInputBindingDescriptions point_input_bindings{};
    // point_input_bindings
    // .add(0, sizeof(std::pair<glm::vec2, glm::vec4>), VK_VERTEX_INPUT_RATE_VERTEX);
    // vkw::pipeline::VertexInputAttributeDescriptions point_input_attributes{};
    // point_input_attributes
    // .add(0, 0, VK_FORMAT_R32G32_SFLOAT, 0)
    // .add(1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec2));
    // vkw::pipeline::VertexInputState point_vertex_input_state(point_input_bindings, point_input_attributes);
    // vkw::pipeline::InputAssemblyState point_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
    // vkw::pipeline::ViewportState point_viewport_state(
    //     {
    //         .x = 0.0f, .y = 0.0f,
    //         .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
    //         .minDepth = 0.0f, .maxDepth = 1.0f
    //     },
    //     {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    // );
    // vkw::pipeline::RasterizarionState point_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    // vkw::pipeline::MultisampleState point_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    // vkw::pipeline::DepthStencilState point_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE);
    // vkw::pipeline::ColorBlendAttachmentStates point_blend_attachment_states{};
    // point_blend_attachment_states
    // .add();
    // vkw::pipeline::ColorBlendState point_color_blend_state(point_blend_attachment_states);

    // vkw::pipeline::GraphicsPipelineStates point_pipeline_state{};
    // point_pipeline_state
    // .shader_stages(point_shader_stages)
    // .vertex_input(point_vertex_input_state)
    // .input_assembly(point_input_assembly_state)
    // .viewport(point_viewport_state)
    // .rasterization(point_rasterization_state)
    // .multisample(point_multisample_state)
    // .depth_stencil(point_depth_stencil_state)
    // .color_blend(point_color_blend_state);

    // auto point_pipeline = device->create_pipeline(point_pipeline_state, point_pipeline_layout, point_render_pass, 0).unwrap();

    std::cerr << std::endl << "create command pool..." << std::endl;
    auto command_pool = device->create_command_pool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, main_queues[0].family_index()).unwrap();

    std::cerr << std::endl << "allocate command buffer..." << std::endl;
    auto command_buffer = command_pool.allocate_command_buffers().unwrap();

    std::cerr << std::endl << "create fence..." << std::endl;
    auto fence = device->create_fence().unwrap();

    vkw::barrier::ImageMemoryBarriers initial_image_barriers{};
    initial_image_barriers
    .add(
        swapchain.image(0), {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
        { VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT },
        { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }
    )
    .add(
        swapchain.image(1), {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
        { VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT },
        { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }
    );

    std::cerr << std::endl << "initialize image layouts..." << std::endl;
    command_buffer.begin_record(0)
    .pipeline_barrier(
        { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT }, 0,
        std::nullopt, std::nullopt, initial_image_barriers
    )
    .end_record();

    vkw::queue::SubmitInfos submit_info{};
    submit_info
    .add(command_buffer);

    main_queues[0].submit(submit_info, fence);

    fence.wait();
    fence.reset();

    command_buffer.reset(0);
    std::cerr << "done." << std::endl;

    auto current_image_index = swapchain.next_image_index(fence);

    game->main_loop(
        [&]() {
            VkRect2D render_area = {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}};
            std::vector<VkClearValue> clear_values = {
                // 0: swapchain image
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
            };

            command_buffer.begin_record(0)
            // line pass
            .begin_render_pass(line_framebuffers[current_image_index], line_render_pass, render_area, clear_values)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, line_pipeline)
            .bind_vertex_buffers(0, {vertex_buffer})
            .bind_index_buffer(index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed(vkw::size_u32(indices.size()), 1)
            .end_render_pass()
            // point pass
            // .begin_render_pass(point_framebuffers[current_image_index], point_render_pass, render_area, {})
            // .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, point_pipeline)
            // .bind_vertex_buffers(0, {point_buffer})
            // .draw(vkw::size_u32(points.size()), 1)
            // .end_render_pass()
            .end_record();

            vkw::queue::SubmitInfos render_submit_info{};
            render_submit_info.add(command_buffer);

            main_queues[0].submit(render_submit_info, fence);

            fence.wait();
            fence.reset();

            command_buffer.reset(0);

            main_queues[0].present({swapchain}, {current_image_index});

            current_image_index = swapchain.next_image_index(fence);
        }
    );

    return 0;
}