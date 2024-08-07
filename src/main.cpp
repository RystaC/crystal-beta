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

    auto instance = std::make_unique<vkw::Instance>();
    {
        auto extensions = app->enum_extensions();
        auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

        auto res = instance->init(extensions, layers);
        if(!res) {
            std::cerr << "[crystal-beta] ERROR: failed to initialize Vulkan instance. exit." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    auto surface = instance->create_surface_SDL(app->window());
    auto physical_device = instance->enum_physical_devices();

    if(physical_device.empty()) {
        std::cerr << "[crystal-beta] ERROR: cannot find physical device on this machine. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for(const auto& pd : physical_device) std::cout << "physical device: " << pd.name() << std::endl;

    auto device = std::make_unique<vkw::Device>();
    {
        auto queue_family_index = physical_device[0].find_queue_family(VK_QUEUE_GRAPHICS_BIT);

        std::vector<const char*> extensions = {
            "VK_KHR_swapchain",
        };
        std::vector<const char*> layers = {
            "VK_LAYER_KHRONOS_validation",
        };

        auto res = device->init(std::move(physical_device[0]), queue_family_index, extensions, layers);
        if(!res) {
            std::cerr << "[crystal-beta] ERROR: failed to initialize Vulkan device. exit." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    auto swapchain = device->create_swapchain(*surface, {VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR}, VK_PRESENT_MODE_FIFO_KHR, WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!swapchain) {
        std::cerr << "[crystal-beta] ERROR: failed to create swapchain. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto depth_buffer = device->create_image(swapchain->width(), swapchain->height(), VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto depth_buffer_view = depth_buffer->create_image_view(VK_IMAGE_ASPECT_DEPTH_BIT);

    auto position_buffer = device->create_image(swapchain->width(), swapchain->height(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto position_output_view = position_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto position_input_view = position_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto normal_buffer = device->create_image(swapchain->width(), swapchain->height(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto normal_output_view = normal_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto normal_input_view = normal_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto albedo_buffer = device->create_image(swapchain->width(), swapchain->height(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
    auto albedo_output_view = albedo_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);
    auto albedo_input_view = albedo_buffer->create_image_view(VK_IMAGE_ASPECT_COLOR_BIT);

    vkw::AttachmentDescriptions attachment_descs{};
    attachment_descs
    // swapchain image
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT, 
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, 
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, 
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    )
    // depth buffer output
    .add(
        VK_FORMAT_D32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    )
    // position buffer output
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    )
    // position buffer input
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    )
    // normal buffer output
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    )
    // normal buffer input
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    )
    // albedo buffer output
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    )
    // albedo buffer input
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    );

    vkw::AttachmentReferences first_output_refs{};
    first_output_refs
    .add(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    .add(4, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    .add(6, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkw::AttachmentReferences second_input_refs{};
    second_input_refs
    .add(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(5, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(7, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkw::AttachmentReferences second_output_refs{};
    second_output_refs
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::SubpassDescriptions subpass_descs{};
    subpass_descs
    // first pass
    .add(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        {},
        first_output_refs,
        {},
        VkAttachmentReference { .attachment = 1, .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
        {}
    )
    // second pass
    .add(
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        second_input_refs,
        second_output_refs,
        {},
        {},
        {}
    );

    vkw::SubpassDependencies depends{};
    depends.add(
        0, 1, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        VK_DEPENDENCY_BY_REGION_BIT
    );

    auto render_pass = device->create_render_pass(attachment_descs, subpass_descs, depends);

    std::vector<std::unique_ptr<vkw::Framebuffer>> framebuffers(swapchain->image_view_size());
    framebuffers[0] = device->create_framebuffer(
        *render_pass,
        {
            swapchain->image_view(0), *depth_buffer_view,
            *position_output_view, *position_input_view,
            *normal_output_view, *normal_input_view,
            *albedo_output_view, *albedo_input_view,
        },
        swapchain->width(), swapchain->height()
    );
    framebuffers[1] = device->create_framebuffer(
        *render_pass,
        {
            swapchain->image_view(1), *depth_buffer_view,
            *position_output_view, *position_input_view,
            *normal_output_view, *normal_input_view,
            *albedo_output_view, *albedo_input_view,
        },
        swapchain->width(), swapchain->height()
    );

    auto first_vertex_shader = device->create_shader_module("shaders/deferred_first.vert.glsl.spirv");
    auto first_fragment_shader = device->create_shader_module("shaders/deferred_first.frag.glsl.spirv");
    auto second_vertex_shader = device->create_shader_module("shaders/deferred_second.vert.glsl.spirv");
    auto second_fragment_shader = device->create_shader_module("shaders/deferred_second.frag.glsl.spirv");

    // auto mesh = meshes::Mesh::rect();
    // auto mesh = meshes::Mesh::cube();
    // auto mesh = meshes::Mesh::sphere(64, 64, 1.0f);
    auto mesh = meshes::Mesh::torus(32, 32, 1.0f, 2.0f);

    std::vector<InstanceBufferData> instance_data = {
        { glm::vec3(-2.0f, 0.0f, 0.0f) },
        { glm::vec3(2.0f, 0.0f, 0.0f) },
    };

    std::vector<UniformBufferData> uniform_data = {
        { 
            glm::vec3(0.0f, 0.0f, -0.5f),
            glm::vec3(0.2f),
            {
                { glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
                { glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
                { glm::vec3(-5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
            },
        }
    };

    auto vertex_buffer = device->create_buffer_with_data(mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto instance_buffer = device->create_buffer_with_data(instance_data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto index_buffer = device->create_buffer_with_data(mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    // auto uniform_buffer = device->create_buffer_with_data(uniform_data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
        { .binding = 0, .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, .pImmutableSamplers = nullptr },
        { .binding = 1, .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, .pImmutableSamplers = nullptr },
        { .binding = 2, .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 1, .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, .pImmutableSamplers = nullptr },
    };

    auto descriptor_layout = device->create_descriptor_layout(layout_bindings);

    std::vector<VkDescriptorPoolSize> pool_sizes = {
        { .type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, .descriptorCount = 3 },
    };

    // auto descriptor_pool = device->create_descriptor_pool(pool_sizes, vkw::size_u32(swapchain->image_view_size()));
    auto descriptor_pool = device->create_descriptor_pool(pool_sizes, 3);

    auto second_descriptor_set = descriptor_pool->allocate_descriptor_set<VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT>(*descriptor_layout);
    std::vector<VkDescriptorImageInfo> image_infos = {
        { .sampler = VK_NULL_HANDLE, .imageView = *position_input_view, .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        { .sampler = VK_NULL_HANDLE, .imageView = *normal_input_view, .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        { .sampler = VK_NULL_HANDLE, .imageView = *albedo_input_view, .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
    };
    second_descriptor_set->update(0, 0, image_infos);

    vkw::VertexInputBindingDescriptions bind_descs{};
    bind_descs
    .add(0, sizeof(meshes::VertexData), VK_VERTEX_INPUT_RATE_VERTEX)
    .add(1, sizeof(InstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE);

    vkw::VertexInputAttributeDescriptions attr_descs{};
    attr_descs
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(meshes::VertexData, normal))
    .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(meshes::VertexData, color))
    .add(3, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceBufferData, translate));

    std::vector<VkViewport> viewports = {
        {0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, 1.0f},
    };
    std::vector<VkRect2D> scissors = {
        {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}},
    };

    std::vector<VkPipelineColorBlendAttachmentState> first_blend_attachment_states = {
        { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
        { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
        { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    };

    std::vector<VkPipelineColorBlendAttachmentState> second_blend_attachment_states = {
        { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    };

    auto first_pipeline_states = vkw::GraphicsPipelineStates()
        .add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *first_vertex_shader, "main")
        .add_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, *first_fragment_shader, "main")
        .vertex_input_state(bind_descs, attr_descs)
        .input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .viewport_state(viewports, scissors)
        .rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f)
        .multisample_state(VK_SAMPLE_COUNT_1_BIT)
        .depth_stencil_state(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE)
        .color_blend_state(first_blend_attachment_states);

    auto second_pipeline_states = vkw::GraphicsPipelineStates()
        .add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *second_vertex_shader, "main")
        .add_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, *second_fragment_shader, "main")
        .input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .viewport_state(viewports, scissors)
        .rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f)
        .multisample_state(VK_SAMPLE_COUNT_1_BIT)
        .depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE)
        .color_blend_state(second_blend_attachment_states);

    std::vector<VkDescriptorSetLayout> descriptor_layouts = {
        *descriptor_layout,
    };

    std::vector<VkPushConstantRange> constant_ranges = {
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0u,
            .size = sizeof(PushConstantData),
        }
    };

    auto first_pipeline = device->create_graphics_pipeline({}, constant_ranges, first_pipeline_states, *render_pass, 0);
    auto second_pipeline = device->create_graphics_pipeline(descriptor_layouts, {}, second_pipeline_states, *render_pass, 1);

    auto command_pool = device->create_command_pool();
    if(!command_pool) {
        std::cerr << "[crystal-beta] ERROR: failed to create command pool. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto command_buffer = command_pool->allocate_command_buffer();
    if(!command_buffer) {
        std::cerr << "[crystal-beta] ERROR: failed to allocate command buffer. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    auto fence = device->create_fence();
    if(!fence) {
        std::cerr << "[crystal-beta] ERROR: failed to create fence. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    command_buffer->record_commands(
        [&](vkw::CommandBufferCommands& c) {
            c
            .barrier_image(swapchain->image(0),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            )
            .barrier_image(swapchain->image(1),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_MEMORY_READ_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            )
            ;
        }
    );

    device->submit_commands(*command_buffer, *fence);
    fence->wait(UINT64_MAX);
    fence->reset();

    uint32_t current_index = swapchain->next_image_index(*fence);

    app->main_loop(
        [&](){
            VkRect2D render_area = {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}};
            std::vector<VkClearValue> clear_values = {
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .depthStencil = {1.0f, 0} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
            };

            glm::mat4 model= glm::rotate(glm::mat4(1.0f), glm::radians((float)app->ticks()), glm::vec3(1.0f));
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
            projection[1][1] *= -1;

            PushConstantData push_constant_data {
                model, view, projection,
            };

            std::vector<VkDescriptorSet> descriptor_sets = {
                *second_descriptor_set 
            };

            command_buffer->record_commands(
                [&](vkw::CommandBufferCommands& c) {
                    c
                    .barrier_image(swapchain->image(current_index),
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    )
                    .begin_render_pass(*(framebuffers[current_index]), *render_pass, render_area, clear_values)
                    // .bind_graphics_pipeline(pipeline->pipeline())
                    .bind_graphics_pipeline(first_pipeline->pipeline())
                    // .bind_descriptor_set(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout(), *descriptor_sets[current_index])
                    // .push_constants(pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(PushConstantData), &push_constant_data)
                    .push_constants(first_pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantData), &push_constant_data)
                    .bind_vertex_buffer(0, *vertex_buffer)
                    .bind_vertex_buffer(1, *instance_buffer)
                    .bind_index_buffer(*index_buffer, VK_INDEX_TYPE_UINT16)
                    .draw_indexed((uint32_t)mesh.indices().size(), 2)
                    .next_subpass()
                    .bind_graphics_pipeline(second_pipeline->pipeline())
                    .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, second_pipeline->layout(), descriptor_sets)
                    .draw(3, 1)
                    .end_render_pass();
                }
            );

            device->submit_commands(*command_buffer, *fence);
            fence->wait(UINT64_MAX);

            device->present(*swapchain, current_index);

            fence->reset();

            current_index = swapchain->next_image_index(*fence);
        }
    );

    return 0;
}