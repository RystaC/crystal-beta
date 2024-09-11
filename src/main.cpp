#include <iostream>
#include <memory>
#include <vector>

#include <cmath>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "App.hpp"
#include "vkw/Instance.hpp"
#include "vkw/Device.hpp"

#include "meshes/Mesh.hpp"

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 480;

struct PushConstantData {
    glm::mat4 model, view, projection;
};

constexpr uint32_t MAX_LIGHTS = 3;

struct UniformBufferData {
    alignas(16) glm::vec3 camera_position;
    alignas(16) glm::vec3 ambient;
    alignas(16) struct {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
    } lights[MAX_LIGHTS];
};

struct InstanceBufferData {
    glm::vec3 translate;
};

int main(int argc, char** argv) {
    auto app = std::make_unique<App>();
    auto result = app->init(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    for(const auto& extension : instance_extensions) {
        std::cerr << extension.extensionName << std::endl;
    }

    for(const auto& layer : instance_layers) {
        std::cerr << std::endl << "enumerate instance extensions for " << layer.layerName << "..." << std::endl;
        const auto layer_extensions = vkw::Instance::enumerate_extensions(layer.layerName);
        for(const auto& extension : layer_extensions) {
            std::cerr << extension.extensionName << std::endl;
        }
    }

    std::cerr << std::endl;

    auto instance = std::make_unique<vkw::Instance>();
    {
        // enum least required extensions
        auto app_extensions = app->enum_extensions();
        // activate validation layer
        auto app_layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

        auto res = instance->init(app_extensions, app_layers);
        if(res) {
            std::cerr << "instance initialization is succeeded." << std::endl;
        }
        else {
            std::cerr << "instance initialization is failed. exit." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    auto surface = instance->create_surface_SDL(app->window());

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
    auto device = std::make_unique<vkw::Device>(physical_devices[0]);
    {
        vkw::queue::CreateInfos queue_create_infos{};
        queue_create_infos.add(queue_family_index, {0.0f});

        std::vector<const char*> extensions = { "VK_KHR_swapchain" };
        std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

        auto res = device->init(queue_create_infos, extensions, layers);
        if(res) {
            std::cerr << "device initialization is succeeded." << std::endl;
        }
        else {
            std::cerr << "device initialization is failed. exit." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    std::cerr << std::endl << "create device queue for graphics..." << std::endl;
    auto main_queues = device->create_queues(queue_family_index);
    std::cerr << "queue family index = " << main_queues[0].family_index() << ", size = " << main_queues.size() << std::endl;

    std::cerr << std::endl << "create swapchain..." << std::endl;
    auto swapchain = device->create_swapchain(surface, queue_family_index, {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, VK_PRESENT_MODE_FIFO_KHR, {WINDOW_WIDTH, WINDOW_HEIGHT});

    std::cerr << std::endl << "create images..." << std::endl;
    auto depth_buffer = device->create_image(swapchain.extent(), VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto depth_buffer_view = depth_buffer.create_image_view(VK_IMAGE_ASPECT_DEPTH_BIT);
    auto position_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto position_buffer_view = position_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto normal_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto normal_buffer_view = normal_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto albedo_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto albedo_buffer_view = albedo_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);

    vkw::render_pass::AttachmentDescriptions deferred_attachment_descs{};
    deferred_attachment_descs
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
    )
    // attachment 2: g-buffer position
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    )
    // attachment 3: g-buffer normal
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    )
    // attachment 4: g-buffer albedo
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    );

    vkw::render_pass::AttachmentReferences geometry_output_refs{};
    geometry_output_refs
    .add(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    .add(3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    .add(4, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkw::render_pass::AttachmentReferences geometry_depth_ref{};
    geometry_depth_ref
    .add(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    vkw::render_pass::AttachmentReferences light_input_refs{};
    light_input_refs
    .add(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkw::render_pass::AttachmentReferences light_output_refs{};
    light_output_refs
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions deferred_subpass_descs{};
    // subpass 0: geometry pass
    deferred_subpass_descs.add()
    .output_color_attachments(geometry_output_refs)
    .output_depth_attachment(geometry_depth_ref)
    .end();
    // subpass 1: lighting pass
    deferred_subpass_descs.add()
    .input_attachments(light_input_refs)
    .output_color_attachments(light_output_refs)
    .end();

    vkw::render_pass::SubpassDependencies deferred_pass_depends{};
    deferred_pass_depends
    .add(
        {0, 1},
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT},
        {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT},
        VK_DEPENDENCY_BY_REGION_BIT
    );

    std::cerr << std::endl << "create render pass..." << std::endl;
    auto deferred_render_pass = device->create_render_pass(deferred_attachment_descs, deferred_subpass_descs, deferred_pass_depends);

    std::cerr << std::endl << "create framebuffers..." << std::endl;
    std::vector<vkw::objects::Framebuffer> deferred_framebuffers(swapchain.size());
    deferred_framebuffers[0] = 
    device->create_framebuffer(
        deferred_render_pass,
        {
            swapchain.image_view(0), depth_buffer_view,
            position_buffer_view, normal_buffer_view, albedo_buffer_view,
        },
        swapchain.extent()
    );
    deferred_framebuffers[1] = 
    device->create_framebuffer(
        deferred_render_pass,
        {
            swapchain.image_view(1), depth_buffer_view,
            position_buffer_view, normal_buffer_view, albedo_buffer_view,
        },
        swapchain.extent()
    );

    std::cerr << std::endl << "create shader modules..." << std::endl;
    auto geometry_vertex_shader = device->create_shader_module("shaders/deferred_first.vert.glsl.spirv");
    auto geometry_fragment_shader = device->create_shader_module("shaders/deferred_first.frag.glsl.spirv");
    auto light_vertex_shader = device->create_shader_module("shaders/deferred_second.vert.glsl.spirv");
    auto light_fragment_shader = device->create_shader_module("shaders/deferred_second.frag.glsl.spirv");

    auto mesh = meshes::Mesh::torus(32, 32, 1.0f, 2.0f);

    std::vector<InstanceBufferData> instance_data = {
        { glm::vec3(-2.0f, 0.0f, 0.0f) },
        { glm::vec3(2.0f, 0.0f, 0.0f) },
    };

    // std::vector<UniformBufferData> uniform_data = {
    //     { 
    //         glm::vec3(0.0f, 0.0f, -0.5f),
    //         glm::vec3(0.2f),
    //         {
    //             { glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
    //             { glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
    //             { glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
    //         },
    //     }
    // };

    std::cerr << std::endl << "create buffers..." << std::endl;
    auto vertex_buffer = device->create_buffer_with_data(mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto instance_buffer = device->create_buffer_with_data(instance_data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto index_buffer = device->create_buffer_with_data(mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    // auto uniform_buffer = device->create_buffer_with_data(uniform_data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    vkw::descriptor::DescriptorSetLayoutBindings light_layout_bindings(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
    light_layout_bindings
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
    .add(1, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
    .add(2, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    std::cerr << std::endl << "create descriptor set layout..." << std::endl;
    auto light_descriptor_layout = device->create_descriptor_set_layout(light_layout_bindings);

    std::cerr << std::endl << "create descriptor pool..." << std::endl;
    auto light_descriptor_pool = device->create_descriptor_pool({light_layout_bindings});

    std::cerr << std::endl << "allocate descriptor sets..." << std::endl;
    auto light_descriptor_sets = light_descriptor_pool.allocate_descriptor_sets({light_descriptor_layout});
    std::cerr << "size of descriptor sets = " << light_descriptor_sets.size() << std::endl;

    vkw::descriptor::ImageInfos light_image_infos{};
    light_image_infos
    .add(position_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(normal_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(albedo_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkw::descriptor::WriteDescriptorSets light_writes{};
    light_writes.add(light_descriptor_sets[0], 0, 0, light_image_infos);

    std::cerr << std::endl << "update descriptor sets..." << std::endl;
    device->update_descriptor_sets(light_writes);

    vkw::pipeline_layout::CreateInfo geometry_layout_info{};
    geometry_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantData));

    vkw::pipeline_layout::CreateInfo light_layout_info{};
    light_layout_info
    .add_descriptor_set_layout(light_descriptor_layout);

    std::cerr << std::endl << "create pipeline layout..." << std::endl;
    auto geometry_pipeline_layout = device->create_pipeline_layout(geometry_layout_info);
    auto light_pipeline_layout = device->create_pipeline_layout(light_layout_info);

    // vkw::VertexInputBindingDescriptions bind_descs{};
    // bind_descs
    // .add(0, sizeof(meshes::VertexData), VK_VERTEX_INPUT_RATE_VERTEX)
    // .add(1, sizeof(InstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE);

    // vkw::VertexInputAttributeDescriptions attr_descs{};
    // attr_descs
    // .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, position))
    // .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, normal))
    // .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(meshes::VertexData, color))
    // .add(3, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceBufferData, translate));

    // std::vector<VkViewport> viewports = {
    //     {0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, 1.0f},
    // };
    // std::vector<VkRect2D> scissors = {
    //     {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}},
    // };

    // std::vector<VkPipelineColorBlendAttachmentState> first_blend_attachment_states = {
    //     { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    //     { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    //     { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    // };

    // std::vector<VkPipelineColorBlendAttachmentState> second_blend_attachment_states = {
    //     { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    // };

    // auto first_pipeline_states = vkw::GraphicsPipelineStates()
    //     .add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *first_vertex_shader, "main")
    //     .add_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, *first_fragment_shader, "main")
    //     .vertex_input_state(bind_descs, attr_descs)
    //     .input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    //     .viewport_state(viewports, scissors)
    //     .rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f)
    //     .multisample_state(VK_SAMPLE_COUNT_1_BIT)
    //     .depth_stencil_state(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE)
    //     .color_blend_state(first_blend_attachment_states);

    // auto second_pipeline_states = vkw::GraphicsPipelineStates()
    //     .add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *second_vertex_shader, "main")
    //     .add_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, *second_fragment_shader, "main")
    //     .input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
    //     .viewport_state(viewports, scissors)
    //     .rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f)
    //     .multisample_state(VK_SAMPLE_COUNT_1_BIT)
    //     .depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE)
    //     .color_blend_state(second_blend_attachment_states);

    // vkw::VertexInputBindingDescriptions debug_bind_descs{};
    // debug_bind_descs
    // .add(0, sizeof(meshes::VertexData), VK_VERTEX_INPUT_RATE_VERTEX)
    // .add(1, sizeof(InstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE)
    // .add(2, sizeof(DebugInstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE);

    // vkw::VertexInputAttributeDescriptions debug_attr_descs{};
    // debug_attr_descs
    // .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, position))
    // .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, normal))
    // .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(meshes::VertexData, color))
    // .add(3, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceBufferData, translate))
    // .add(4, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugInstanceBufferData, scale));

    // auto first_pipeline = device->create_graphics_pipeline({}, constant_ranges, first_pipeline_states, *render_pass, 0);
    // auto second_pipeline = device->create_graphics_pipeline(descriptor_layouts, {}, second_pipeline_states, *render_pass, 1);

    // auto command_pool = device->create_command_pool();
    // if(!command_pool) {
    //     std::cerr << "[crystal-beta] ERROR: failed to create command pool. exit." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // auto command_buffer = command_pool->allocate_command_buffer();
    // if(!command_buffer) {
    //     std::cerr << "[crystal-beta] ERROR: failed to allocate command buffer. exit." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // auto fence = device->create_fence();
    // if(!fence) {
    //     std::cerr << "[crystal-beta] ERROR: failed to create fence. exit." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // command_buffer->record_commands(
    //     [&](vkw::CommandBufferCommands& c) {
    //         c
    //         .barrier_image(swapchain->image(0),
    //             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    //             VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT,
    //             VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    //         )
    //         .barrier_image(swapchain->image(1),
    //             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    //             VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT,
    //             VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    //         )
    //         ;
    //     }
    // );

    // device->submit_commands(*command_buffer, *fence);
    // fence->wait(UINT64_MAX);
    // fence->reset();

    // uint32_t current_index = swapchain->next_image_index(*fence);

    // app->main_loop(
    //     [&](){
    //         VkRect2D render_area = {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}};
    //         std::vector<VkClearValue> clear_values = {
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .depthStencil = {1.0f, 0} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //         };

    //         std::vector<VkClearValue> debug_clear_values = {
    //             { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
    //         };

    //         glm::mat4 model= glm::rotate(glm::mat4(1.0f), glm::radians((float)app->ticks()), glm::vec3(1.0f));
    //         glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //         glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
    //         projection[1][1] *= -1;

    //         PushConstantData push_constant_data {
    //             model, view, projection,
    //         };

    //         std::vector<VkDescriptorSet> descriptor_sets = {
    //             *second_descriptor_set 
    //         };

    //         command_buffer->record_commands(
    //             [&](vkw::CommandBufferCommands& c) {
    //                 c
    //                 .begin_render_pass(*(framebuffers[current_index]), *render_pass, render_area, clear_values)
    //                 // .bind_graphics_pipeline(pipeline->pipeline())
    //                 .bind_graphics_pipeline(first_pipeline->pipeline())
    //                 // .bind_descriptor_set(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout(), *descriptor_sets[current_index])
    //                 // .push_constants(pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(PushConstantData), &push_constant_data)
    //                 .push_constants(first_pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantData), &push_constant_data)
    //                 .bind_vertex_buffer(0, *vertex_buffer)
    //                 .bind_vertex_buffer(1, *instance_buffer)
    //                 .bind_index_buffer(*index_buffer, VK_INDEX_TYPE_UINT16)
    //                 .draw_indexed((uint32_t)mesh.indices().size(), 2)
    //                 .next_subpass()
    //                 .bind_graphics_pipeline(second_pipeline->pipeline())
    //                 .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, second_pipeline->layout(), descriptor_sets)
    //                 .draw(3, 1)
    //                 .end_render_pass()
    //             }
    //         );

    //         device->submit_commands(*command_buffer, *fence);
    //         fence->wait(UINT64_MAX);

    //         device->present(*swapchain, current_index);

    //         fence->reset();

    //         current_index = swapchain->next_image_index(*fence);
    //     }
    // );

    app->main_loop([&](){});

    return 0;
}