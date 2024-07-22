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

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 480;

constexpr float PI = 3.14159265359f;

struct PushConstantData {
    glm::mat4 model, view, projection;
};

struct VertexBufferData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
};

std::pair<std::vector<VertexBufferData>, std::vector<uint16_t>> create_torus(uint32_t row, uint32_t column, float inner_rad, float outer_rad) {
    std::vector<VertexBufferData> vertices{};
    std::vector<uint16_t> indices{};
    vertices.reserve((row+1)*(column+1));
    indices.reserve(row*column*6);

    for(auto i = 0u; i <= row; ++i) {
        float r = PI * 2.0f / row * i;
        float rr = std::cos(r);
        float ry = std::sin(r);
        for(auto ii = 0u; ii <= column; ++ii) {
            float tr = PI * 2.0f / column * ii;
            float tx = (rr * inner_rad + outer_rad) * std::cos(tr);
            float ty = ry * inner_rad;
            float tz = (rr * inner_rad + outer_rad) * std::sin(tr);
            float rx = rr * std::cos(tr);
            float rz = rr * std::sin(tr);
            vertices.emplace_back(VertexBufferData{glm::vec3(tx, ty, tz), glm::vec3(rx, ry, rz), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)});
        }
    }

    for(auto i = 0u; i < row; ++i) {
        for(auto ii = 0u; ii < column; ++ii) {
            uint16_t r = (column + 1) * i + ii;
            indices.push_back(r);
            indices.push_back(r + column + 1);
            indices.push_back(r + 1);
            indices.push_back(r + column + 1);
            indices.push_back(r + column + 2);
            indices.push_back(r + 1);
        }
    }

    return {vertices, indices};
}

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

    auto render_pass = device->create_render_pass();

    std::vector<std::unique_ptr<vkw::Framebuffer>> framebuffers(swapchain->image_view_size());
    framebuffers[0] = device->create_framebuffer(*render_pass, {swapchain->image_view(0)}, swapchain->width(), swapchain->height());
    framebuffers[1] = device->create_framebuffer(*render_pass, {swapchain->image_view(1)}, swapchain->width(), swapchain->height());

    auto vertex_shader = device->create_shader_module("basic.vert.spirv");
    auto fragment_shader = device->create_shader_module("basic.frag.spirv");

    // std::vector<glm::vec3> vertices = {
    //     glm::vec3(-0.5f, 0.5f, 0.0f),
    //     glm::vec3(-0.5f, -0.5f, 0.0f),
    //     glm::vec3(0.5f, 0.5f, 0.0f),
    //     glm::vec3(0.5f, -0.5f, 0.0f),
    // };
    // std::vector<uint16_t> indices = {
    //     0, 1, 2,
    //     1, 3, 2,
    // };

    auto [vertices, indices] = create_torus(32, 32, 1.0f, 2.0f);

    auto vertex_buffer = device->create_buffer_with_data(vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto index_buffer = device->create_buffer_with_data(indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    std::vector<VkVertexInputBindingDescription> bind_descs {
        { .binding = 0, .stride = sizeof(VertexBufferData), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX},
    };
    std::vector<VkVertexInputAttributeDescription> attr_descs {
        { .location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(VertexBufferData, position) },
        { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(VertexBufferData, normal) },
        { .location = 2, .binding = 0, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = offsetof(VertexBufferData, color) }
    };

    std::vector<VkViewport> viewports = {
        {0.0f, 0.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.0f, 1.0f},
    };
    std::vector<VkRect2D> scissors = {
        {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}},
    };

    std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = {
        { .blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    };

    auto pipeline_states = vkw::GraphicsPipelineStates()
        .add_shader_stage(VK_SHADER_STAGE_VERTEX_BIT, *vertex_shader, "main")
        .add_shader_stage(VK_SHADER_STAGE_FRAGMENT_BIT, *fragment_shader, "main")
        .vertex_input_state(bind_descs, attr_descs)
        .input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .viewport_state(viewports, scissors)
        .rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f)
        .multisample_state(VK_SAMPLE_COUNT_1_BIT)
        .color_blend_state(blend_attachment_states);

    std::vector<VkPushConstantRange> constant_ranges = {
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0u,
            .size = sizeof(PushConstantData),
        }
    };

    auto pipeline = device->create_graphics_pipeline({}, constant_ranges, pipeline_states, *render_pass, 0);
    if(!pipeline) {
        std::cerr << "[crystal-beta] ERROR: failed to create pipeline. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

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
            VkClearValue clear_value = {0.0f, 0.0f, 0.0f, 1.0f};

            glm::mat4 model= glm::rotate(glm::mat4(1.0f), glm::radians((float)app->ticks()), glm::vec3(1.0f));
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
            projection[1][1] *= -1;

            PushConstantData push_constant_data {
                model, view, projection,
            };

            command_buffer->record_commands(
                [&](vkw::CommandBufferCommands& c) {
                    c
                    .barrier_image(swapchain->image(current_index),
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                        VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    )
                    .begin_render_pass(*(framebuffers[current_index]), *render_pass, render_area, {clear_value})
                    .bind_graphics_pipeline(pipeline->pipeline())
                    .push_constants(pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(PushConstantData), &push_constant_data)
                    .bind_vertex_buffer(*vertex_buffer)
                    .bind_index_buffer(*index_buffer, VK_INDEX_TYPE_UINT16)
                    .draw_indexed((uint32_t)indices.size(), 1)
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