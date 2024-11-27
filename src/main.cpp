#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <cmath>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game.hpp"

#include "vkw/vkw.hpp"

#include "mesh/BasicMesh.hpp"
#include "mesh/Obj.hpp"
#include "mesh/PMX.hpp"

constexpr size_t WINDOW_WIDTH = 1280;
constexpr size_t WINDOW_HEIGHT = 720;
// constexpr size_t WINDOW_WIDTH = 512;
// constexpr size_t WINDOW_HEIGHT = 512;

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

class TestCustomAllocator {
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
        // std::cerr << std::format("[custom allocator] allocated host memory: at {} with size = {}, alignment = {}, scope = {}", p, size, alignment, get_scope_str_(scope)) << std::endl;
        return p;
    }
    static void* VKAPI_CALL reallocation_(void* user_data, void* original, size_t size, size_t alignment, VkSystemAllocationScope scope) {
        auto p = _aligned_realloc(original, size, alignment);
        // std::cerr << std::format("[custom allocator] reallocated host memory: from {}, to {} with size = {}, alignment = {}, scope = {}", original, p, size, alignment, get_scope_str_(scope)) << std::endl;
        return p;
    }
    static void VKAPI_CALL free_(void* user_data, void* memory) {
        _aligned_free(memory);
        // std::cerr << std::format("[custom allocator] freed host memory: at {}", memory) << std::endl;
    }

    // notification function is not useful for debug (?)
    // static void VKAPI_CALL allocation_notification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {}
    // static void VKAPI_CALL free_notification_(void* user_data, size_t size, VkInternalAllocationType type, VkSystemAllocationScope scope) {}

public:
    TestCustomAllocator() noexcept : callbacks_ {
        .pUserData = nullptr,
        .pfnAllocation = reinterpret_cast<PFN_vkAllocationFunction>(allocation_),
        .pfnReallocation = reinterpret_cast<PFN_vkReallocationFunction>(reallocation_),
        .pfnFree = reinterpret_cast<PFN_vkFreeFunction>(free_),
        // .pfnInternalAllocation = reinterpret_cast<PFN_vkInternalAllocationNotification>(allocation_notification_),
        // .pfnInternalFree = reinterpret_cast<PFN_vkInternalFreeNotification>(free_notification_),
    } {}
    operator const VkAllocationCallbacks*() const noexcept { return &callbacks_; }
};

int main(int argc, char** argv) {
    // auto s = std::chrono::high_resolution_clock::now();
    // auto mesh = mesh::Obj::load("../asset/obj/bunny.obj");
    // auto e = std::chrono::high_resolution_clock::now();
    // auto time = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    // std::cerr << std::format("mesh::Obj::load(): {} msec", time) << std::endl;
    mesh::PMX pmx{};

    try {
        pmx = mesh::PMX::load("../asset/mmd/Tda式改変ミク　JKStyle/Tda式改変ミク　JKStyle.pmx");
    }
    catch(std::exception& e) {
        std::cerr << std::format("exception occured. {}", e.what());
        std::exit(EXIT_FAILURE);
    }

    // auto mesh = mesh::BasicMesh::sphere(8, 8, 1.0f);

    auto game = std::make_unique<Game>();
    auto result = game->init(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!result) {
        std::cerr << "[crystal-beta] ERROR: failed to initialize application. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto version = vkw::Instance::version();
    std::cerr << "Vulkan version: " << VK_API_VERSION_MAJOR(version) << "." << VK_API_VERSION_MINOR(version) << "." << VK_API_VERSION_PATCH(version) << ", variant = " << VK_API_VERSION_VARIANT(version) << std::endl;
    
    std::cerr << std::endl << "enumerate instance layers..." << std::endl;
    const auto instance_layers = vkw::Instance::enumerate_layers();
    for(const auto& layer : instance_layers) {
        std::cerr << layer.layerName << std::endl;
    }

    std::cerr << std::endl << "enumerate default instance extensions..." << std::endl;
    const auto instance_extensions = vkw::Instance::enumerate_extensions();
    bool enable_debug_utils = false;
    for(const auto& extension : instance_extensions) {
        std::cerr << extension.extensionName << std::endl;
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
        std::cerr << std::endl << "enumerate instance extensions for " << layer.layerName << "..." << std::endl;
        const auto layer_extensions = vkw::Instance::enumerate_extensions(layer.layerName);
        for(const auto& extension : layer_extensions) {
            std::cerr << extension.extensionName << std::endl;
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

    std::cerr << std::endl << "enumerate device layers..." << std::endl;
    const auto device_layers = vkw::Device::enumerate_layers(physical_devices[0]);
    for(const auto& layer : device_layers) {
        std::cerr << layer.layerName << std::endl;
    }

    std::cerr << std::endl << "enumerate default device extensions..." << std::endl;
    const auto device_extensions = vkw::Device::enumerate_extensions(physical_devices[0]);
    for(const auto& extension : device_extensions) {
        std::cerr << extension.extensionName << std::endl;
    }

    for(const auto& layer : device_layers) {
        std::cerr << std::endl << "enumerate device extensions for " << layer.layerName << "..." << std::endl;
        const auto layer_extensions = vkw::Device::enumerate_extensions(physical_devices[0], layer.layerName);
        for(const auto& extension : layer_extensions) {
            std::cerr << extension.extensionName << std::endl;
        }
    }

    std::cerr << std::endl;

    auto queue_family_index = physical_devices[0].find_queue_family(VK_QUEUE_GRAPHICS_BIT);

    vkw::queue::CreateInfos queue_create_infos{};
    queue_create_infos.add(queue_family_index, {0.0f});

    std::vector<const char*> desired_device_extensions = { "VK_KHR_swapchain" };
    std::vector<const char*> desired_device_layers = { "VK_LAYER_KHRONOS_validation" };

    VkPhysicalDeviceFeatures device_features {
        .tessellationShader = VK_TRUE,
        .fillModeNonSolid = VK_TRUE,
    };

    auto device = std::make_unique<vkw::Device>(vkw::Device::init(physical_devices[0], device_features, queue_create_infos, desired_device_extensions, desired_device_layers, nullptr).unwrap());


    std::cerr << std::endl << "create device queue for graphics..." << std::endl;
    auto main_queues = device->create_queues(queue_family_index);
    std::cerr << "queue family index = " << main_queues[0].family_index() << ", size = " << main_queues.size() << std::endl;

    std::cerr << std::endl << "create swapchain..." << std::endl;
    auto swapchain = device->create_swapchain(surface, queue_family_index, {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, VK_PRESENT_MODE_FIFO_KHR, {WINDOW_WIDTH, WINDOW_HEIGHT}).unwrap();

    std::cerr << std::endl << "create images..." << std::endl;
    // images for rendering
    auto depth_buffer = device->create_image(swapchain.extent(), VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT).unwrap();
    auto depth_buffer_view = depth_buffer.create_image_view(VK_IMAGE_ASPECT_DEPTH_BIT).unwrap();

    // forward render pass
    vkw::render_pass::AttachmentDescriptions forward_attachment{};
    forward_attachment
    // attachment 0: swapchain image
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    )
    // attachment 1: depth buffer
    .add(
        VK_FORMAT_D32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
    );

    vkw::render_pass::AttachmentReferences forward_attachment_ref{};
    forward_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkw::render_pass::AttachmentReferences forward_depth_ref{};
    forward_depth_ref
    .add(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions forward_subpass{};
    forward_subpass.add()
    .output_color_attachments(forward_attachment_ref)
    .output_depth_attachment(forward_depth_ref)
    .end();

    std::cerr << std::endl << "create render pass..." << std::endl;
    auto forward_render_pass = device->create_render_pass(forward_attachment, forward_subpass).unwrap();

    std::cerr << std::endl << "create framebuffers..." << std::endl;
    std::vector<vkw::resource::Framebuffer> forward_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        forward_framebuffers[i] = device->create_framebuffer(forward_render_pass, {swapchain.image_view(i), depth_buffer_view}, swapchain.extent()).unwrap();
    }

    std::cerr << std::endl << "create shader modules..." << std::endl;
    // auto forward_vertex_shader = device->create_shader_module("spirv/uv_expand.vert.glsl.spirv").unwrap();
    // auto forward_fragment_shader = device->create_shader_module("spirv/uv_expand.frag.glsl.spirv").unwrap();
    // auto forward_vertex_shader = device->create_shader_module("spirv/tessellation_test.vert.glsl.spirv").unwrap();
    // auto forward_fragment_shader = device->create_shader_module("spirv/tessellation_test.frag.glsl.spirv").unwrap();
    // auto forward_tess_cont_shader = device->create_shader_module("spirv/tessellation_test.tesc.glsl.spirv").unwrap();
    // auto forward_tess_eval_shader = device->create_shader_module("spirv/tessellation_test.tese.glsl.spirv").unwrap();
    auto forward_vertex_shader = device->create_shader_module("spirv/forward.vert.glsl.spirv").unwrap();
    auto forward_fragment_shader = device->create_shader_module("spirv/forward.frag.glsl.spirv").unwrap();

    std::cerr << std::endl << "create buffers..." << std::endl;
    // auto vertex_buffer = device->create_buffer_with_data(mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    // auto index_buffer = device->create_buffer_with_data(mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto vertex_buffer = device->create_buffer_with_data(pmx.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto index_buffer = device->create_buffer_with_data(pmx.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();

    vkw::pipeline_layout::CreateInfo forward_layout_info{};
    forward_layout_info
    // .add_push_constant_range(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, 0, sizeof(ForwardConstantData));
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData));

    std::cerr << std::endl << "create pipeline layout..." << std::endl;
    auto forward_pipeline_layout = device->create_pipeline_layout(forward_layout_info).unwrap();

    std::cerr << std::endl << "create pipelines..." << std::endl;

    vkw::pipeline::GraphicsShaderStages forward_shader_stages{};
    forward_shader_stages
    .vertex_shader(forward_vertex_shader)
    // .tessellation_control_shader(forward_tess_cont_shader)
    // .tessellation_evaluation_shader(forward_tess_eval_shader)
    .fragment_shader(forward_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions forward_input_bindings{};
    forward_input_bindings
    // .add(0, sizeof(mesh::VertexAttribute), VK_VERTEX_INPUT_RATE_VERTEX);
    .add(0, sizeof(mesh::pmx::Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    vkw::pipeline::VertexInputAttributeDescriptions forward_input_attributes{};
    forward_input_attributes
    // .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, position))
    // .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, normal))
    // .add(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(mesh::VertexAttribute, tex_coord))
    // .add(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexAttribute, color));
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::pmx::Vertex, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::pmx::Vertex, normal))
    .add(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(mesh::pmx::Vertex, uv));
    vkw::pipeline::VertexInputState forward_vertex_input_state(forward_input_bindings, forward_input_attributes);
    vkw::pipeline::InputAssemblyState forward_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    // vkw::pipeline::InputAssemblyState forward_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST);
    // vkw::pipeline::TessellationState forward_tessellation_state(3);
    vkw::pipeline::ViewportState forward_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState forward_rasterization_state(VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    // vkw::pipeline::RasterizarionState forward_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState forward_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState forward_depth_stencil_state(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates forward_blend_attachment_states{};
    forward_blend_attachment_states
    .add();
    vkw::pipeline::ColorBlendState geometory_color_blend_state(forward_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates forward_pipeline_state{};
    forward_pipeline_state
    .shader_stages(forward_shader_stages)
    .vertex_input(forward_vertex_input_state)
    .input_assembly(forward_input_assembly_state)
    // .tessellation(forward_tessellation_state)
    .viewport(forward_viewport_state)
    .rasterization(forward_rasterization_state)
    .multisample(forward_multisample_state)
    .depth_stencil(forward_depth_stencil_state)
    .color_blend(geometory_color_blend_state);

    auto forward_pipeline = device->create_pipeline(forward_pipeline_state, forward_pipeline_layout, forward_render_pass, 0).unwrap();

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

    float model_rotate = 0.0f;

    game->main_loop(
        [&]() {
            VkRect2D render_area = {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}};
            std::vector<VkClearValue> clear_values = {
                // 0: swapchain image
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                // 1: depth buffer
                { .depthStencil = {1.0f, 0} },
            };

            model_rotate += std::numbers::pi_v<float> * game->delta_time();

            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), model_rotate, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 translate1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 1.0f));
            glm::mat4 translate2 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
            auto camera_pos = game->camera_pos();
            auto camera_dir = game->camera_dir();
            auto camera_up = game->camera_up();
            glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_dir, camera_up);
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
            projection[1][1] *= -1;

            ForwardConstantData push_constant_data1 {
                translate1 * rotate *  model, view, projection,
            };

            ForwardConstantData push_constant_data2 {
                translate2 * rotate * model, view, projection,
            };

            command_buffer.begin_record(0)
            // forward pass
            .begin_render_pass(forward_framebuffers[current_image_index], forward_render_pass, render_area, clear_values)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, forward_pipeline)
            // .push_constants(forward_pipeline_layout, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, 0, sizeof(ForwardConstantData), &push_constant_data1)
            .push_constants(forward_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData), &push_constant_data1)
            .bind_vertex_buffers(0, {vertex_buffer})
            .bind_index_buffer(index_buffer, VK_INDEX_TYPE_UINT32)
            // .draw_indexed(vkw::size_u32(mesh.indices().size()), 1)
            .draw_indexed(vkw::size_u32(pmx.indices().size()), 1)
            // .push_constants(forward_pipeline_layout, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, 0, sizeof(ForwardConstantData), &push_constant_data2)
            // .draw_indexed(vkw::size_u32(mesh.indices().size()), 1)
            .end_render_pass()
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