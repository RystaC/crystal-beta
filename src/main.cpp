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
#include "mesh/HalfEdge.hpp"
#include "mesh/Meshlet.hpp"

constexpr size_t WINDOW_WIDTH = 1280;
constexpr size_t WINDOW_HEIGHT = 720;

struct ForwardConstantData {
    glm::mat4 model, view, projection;
};

struct CullingConstantData {
    glm::vec4 frustum_planes[6];
    glm::mat4 model;
    mesh::AABB box;
};

struct FrustumConstantData {
    glm::mat4 view_proj;
    glm::mat4 inv_view_proj;
};

struct AABBInstanceData {
    alignas(16) glm::vec3 translate;
    alignas(16) glm::vec3 scale;
};

int main(int argc, char** argv) {
    auto s = std::chrono::high_resolution_clock::now();
    auto bunny = mesh::Obj::load("../asset/obj/bunny.obj");
    auto e = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(e - s).count();
    std::cerr << std::format("mesh::Obj::load(): {} msec", time) << std::endl;
    bunny.print_statistics();
    return 0;

    auto meshlet = mesh::Meshlet::generate_meshlet_kdtree(bunny.vertices(), bunny.indices());
    meshlet.print_statistics(bunny.vertices(), bunny.indices());

    std::vector<mesh::Meshlet::Data> bunny_meshlet(meshlet.meshlets().size());
    std::vector<AABBInstanceData> bunny_aabbs(meshlet.meshlets().size());
    std::vector<AABBInstanceData> meshlet_aabbs(meshlet.meshlets().size());

    for(size_t i = 0, i_i = 0; i < meshlet.meshlets().size(); ++i) {
        mesh::AABB box{glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::lowest())};
        bunny_meshlet[i].index_offset = vkw::size_u32(i_i);
        bunny_meshlet[i].index_count = meshlet.meshlets()[i].index_count;
        for(size_t j = 0; j < meshlet.meshlets()[i].index_count; ++j) {
            bunny.vertices()[bunny.indices()[i_i]].color = mesh::hash_color(static_cast<uint32_t>(i));
            box.min = glm::min(box.min, bunny.vertices()[bunny.indices()[i_i]].position);
            box.max = glm::max(box.max, bunny.vertices()[bunny.indices()[i_i]].position);
            i_i += 1;
        }
        bunny_meshlet[i].aabb_min = box.min;
        bunny_meshlet[i].aabb_max = box.max;
    }

    for(size_t i = 0, i_i = 0; i < meshlet.meshlets().size(); ++i) {
        for(size_t j = 0; j < meshlet.meshlets()[i].index_count; ++j) {
            meshlet.vertices()[meshlet.indices()[i_i]].color = mesh::hash_color(static_cast<uint32_t>(i));
            i_i += 1;
        }
    }

    auto aabb_mesh = mesh::BasicMesh::frame();
    auto frustum_mesh = mesh::BasicMesh::cube();

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

    // enum least required extensions
    auto desired_instance_extensions = game->enum_instance_extensions();
    // activate validation layer
    auto desired_instance_layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };
    auto instance = std::make_unique<vkw::Instance>(vkw::Instance::init(desired_instance_extensions, desired_instance_layers).unwrap());

    auto surface = instance->create_surface_SDL(game->window()).unwrap();

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

    auto device = std::make_unique<vkw::Device>(vkw::Device::init(physical_devices[0], queue_create_infos, desired_device_extensions, desired_device_layers).unwrap());


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

    // debug bounding box render pass
    vkw::render_pass::AttachmentDescriptions aabb_attachment{};
    aabb_attachment
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    );

    vkw::render_pass::AttachmentReferences aabb_attachment_ref{};
    aabb_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions aabb_subpass{};
    aabb_subpass.add()
    .output_color_attachments(aabb_attachment_ref)
    .end();

    // debug frustum render pass
    vkw::render_pass::AttachmentDescriptions frustum_attachment{};
    frustum_attachment
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    );

    vkw::render_pass::AttachmentReferences frustum_attachment_ref{};
    frustum_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions frustum_subpass{};
    frustum_subpass.add()
    .output_color_attachments(frustum_attachment_ref)
    .end();

    std::cerr << std::endl << "create render pass..." << std::endl;
    auto forward_render_pass = device->create_render_pass(forward_attachment, forward_subpass).unwrap();
    auto aabb_render_pass = device->create_render_pass(aabb_attachment, aabb_subpass).unwrap();
    auto frustum_render_pass = device->create_render_pass(frustum_attachment, frustum_subpass).unwrap();

    std::cerr << std::endl << "create framebuffers..." << std::endl;
    std::vector<vkw::resource::Framebuffer> forward_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        forward_framebuffers[i] = device->create_framebuffer(forward_render_pass, {swapchain.image_view(i), depth_buffer_view}, swapchain.extent()).unwrap();
    }

    std::vector<vkw::resource::Framebuffer> aabb_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        aabb_framebuffers[i] = device->create_framebuffer(aabb_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    std::vector<vkw::resource::Framebuffer> frustum_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        frustum_framebuffers[i] = device->create_framebuffer(frustum_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    std::cerr << std::endl << "create shader modules..." << std::endl;
    auto forward_vertex_shader = device->create_shader_module("spirv/forward.vert.glsl.spirv").unwrap();
    auto forward_fragment_shader = device->create_shader_module("spirv/forward.frag.glsl.spirv").unwrap();
    auto aabb_vertex_shader = device->create_shader_module("spirv/debug_bounding_box.vert.glsl.spirv").unwrap();
    auto aabb_fragment_shader = device->create_shader_module("spirv/debug_bounding_box.frag.glsl.spirv").unwrap();
    auto frustum_vertex_shader = device->create_shader_module("spirv/debug_frustum.vert.glsl.spirv").unwrap();
    auto frustum_fragment_shader = device->create_shader_module("spirv/debug_frustum.frag.glsl.spirv").unwrap();
    auto aabb_transform_shader = device->create_shader_module("spirv/bounding_box_transform.comp.glsl.spirv").unwrap();

    std::cerr << std::endl << "create buffers..." << std::endl;
    auto vertex_buffer = device->create_buffer_with_data(bunny.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto index_buffer = device->create_buffer_with_data(bunny.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto meshlet_vertex_buffer = device->create_buffer_with_data(meshlet.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto meshlet_index_buffer = device->create_buffer_with_data(meshlet.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto aabb_vertex_buffer = device->create_buffer_with_data(aabb_mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto aabb_index_buffer = device->create_buffer_with_data(aabb_mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto frustum_vertex_buffer = device->create_buffer_with_data(frustum_mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto frustum_index_buffer = device->create_buffer_with_data(frustum_mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto bunny_meshlet_input_buffer = device->create_buffer_with_data(bunny_meshlet, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT).unwrap();
    auto meshlet_meshlet_input_buffer = device->create_buffer_with_data(meshlet.meshlets(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT).unwrap();
    auto bunny_aabb_instance_buffer = device->create_buffer_with_data(bunny_aabbs, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto meshlet_aabb_instance_buffer = device->create_buffer_with_data(meshlet_aabbs, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();

    vkw::descriptor::DescriptorSetLayoutBindings aabb_layout_bindings(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    aabb_layout_bindings
    .add(0, 1, VK_SHADER_STAGE_COMPUTE_BIT)
    .add(1, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    auto aabb_descriptor_layout = device->create_descriptor_set_layout(aabb_layout_bindings).unwrap();

    auto aabb_descriptor_pool = device->create_descriptor_pool({aabb_layout_bindings, aabb_layout_bindings}).unwrap();
    auto aabb_descriptor_sets = aabb_descriptor_pool.allocate_descriptor_sets({aabb_descriptor_layout, aabb_descriptor_layout}).unwrap();

    vkw::descriptor::BufferInfos bunny_aabb_infos{};
    bunny_aabb_infos
    .add(bunny_meshlet_input_buffer, 0, sizeof(mesh::Meshlet::Data) * bunny_meshlet.size())
    .add(bunny_aabb_instance_buffer, 0, sizeof(AABBInstanceData) * bunny_meshlet.size());

    vkw::descriptor::BufferInfos meshlet_aabb_infos{};
    meshlet_aabb_infos
    .add(meshlet_meshlet_input_buffer, 0, sizeof(mesh::Meshlet::Data) * meshlet.meshlets().size())
    .add(meshlet_aabb_instance_buffer, 0, sizeof(AABBInstanceData) * meshlet.meshlets().size());

    vkw::descriptor::WriteDescriptorSets descriptor_writes{};
    descriptor_writes
    .add(aabb_descriptor_sets[0], 0, 0, bunny_aabb_infos)
    .add(aabb_descriptor_sets[1], 0, 0, meshlet_aabb_infos);

    device->update_descriptor_sets(descriptor_writes);

    vkw::pipeline_layout::CreateInfo forward_layout_info{};
    forward_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData));

    vkw::pipeline_layout::CreateInfo aabb_layout_info{};
    aabb_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData));

    vkw::pipeline_layout::CreateInfo frustum_layout_info{};
    frustum_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FrustumConstantData));

    vkw::pipeline_layout::CreateInfo aabb_trans_layout_info{};
    aabb_trans_layout_info
    .add_descriptor_set_layout(aabb_descriptor_layout)
    .add_push_constant_range(VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ForwardConstantData));

    std::cerr << std::endl << "create pipeline layout..." << std::endl;
    auto forward_pipeline_layout = device->create_pipeline_layout(forward_layout_info).unwrap();
    auto aabb_pipeline_layout = device->create_pipeline_layout(aabb_layout_info).unwrap();
    auto frustum_pipeline_layout = device->create_pipeline_layout(frustum_layout_info).unwrap();
    auto aabb_trans_pipeline_layout = device->create_pipeline_layout(aabb_trans_layout_info).unwrap();

    std::cerr << std::endl << "create pipelines..." << std::endl;

    vkw::pipeline::GraphicsShaderStages forward_shader_stages{};
    forward_shader_stages
    .vertex_shader(forward_vertex_shader)
    .fragment_shader(forward_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions forward_input_bindings{};
    forward_input_bindings
    .add(0, sizeof(mesh::VertexAttribute), VK_VERTEX_INPUT_RATE_VERTEX);
    vkw::pipeline::VertexInputAttributeDescriptions forward_input_attributes{};
    forward_input_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, normal))
    .add(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(mesh::VertexAttribute, tex_coord))
    .add(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexAttribute, color));
    vkw::pipeline::VertexInputState forward_vertex_input_state(forward_input_bindings, forward_input_attributes);
    vkw::pipeline::InputAssemblyState forward_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState forward_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState forward_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
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
    .viewport(forward_viewport_state)
    .rasterization(forward_rasterization_state)
    .multisample(forward_multisample_state)
    .depth_stencil(forward_depth_stencil_state)
    .color_blend(geometory_color_blend_state);

    auto forward_pipeline = device->create_pipeline(forward_pipeline_state, forward_pipeline_layout, forward_render_pass, 0).unwrap();

    // debug bounding box pipeline
    vkw::pipeline::GraphicsShaderStages aabb_shader_stages{};
    aabb_shader_stages
    .vertex_shader(aabb_vertex_shader)
    .fragment_shader(aabb_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions aabb_vertex_bindings{};
    aabb_vertex_bindings
    .add(0, sizeof(mesh::VertexAttribute), VK_VERTEX_INPUT_RATE_VERTEX)
    .add(1, sizeof(AABBInstanceData), VK_VERTEX_INPUT_RATE_INSTANCE);
    vkw::pipeline::VertexInputAttributeDescriptions aabb_vertex_attributes{};
    aabb_vertex_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, normal))
    .add(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(mesh::VertexAttribute, tex_coord))
    .add(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexAttribute, color))
    .add(4, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(AABBInstanceData, translate))
    .add(5, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(AABBInstanceData, scale));
    vkw::pipeline::VertexInputState aabb_vertex_input_state(aabb_vertex_bindings, aabb_vertex_attributes);
    vkw::pipeline::InputAssemblyState aabb_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    vkw::pipeline::ViewportState aabb_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState aabb_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState aabb_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState aabb_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates aabb_blend_attachment_states{};
    aabb_blend_attachment_states.add(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);
    vkw::pipeline::ColorBlendState aabb_color_blend_state(aabb_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates aabb_pipeline_states{};
    aabb_pipeline_states
    .shader_stages(aabb_shader_stages)
    .vertex_input(aabb_vertex_input_state)
    .input_assembly(aabb_input_assembly_state)
    .viewport(aabb_viewport_state)
    .rasterization(aabb_rasterization_state)
    .multisample(aabb_multisample_state)
    .depth_stencil(aabb_depth_stencil_state)
    .color_blend(aabb_color_blend_state);

    auto aabb_pipeline = device->create_pipeline(aabb_pipeline_states, aabb_pipeline_layout, aabb_render_pass, 0).unwrap();

    // debug frustum pipeline
    vkw::pipeline::GraphicsShaderStages frustum_shader_stages{};
    frustum_shader_stages
    .vertex_shader(frustum_vertex_shader)
    .fragment_shader(frustum_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions frustum_vertex_bindings{};
    frustum_vertex_bindings
    .add(0, sizeof(mesh::VertexAttribute), VK_VERTEX_INPUT_RATE_VERTEX);
    vkw::pipeline::VertexInputAttributeDescriptions frustum_vertex_attributes{};
    frustum_vertex_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexAttribute, normal))
    .add(2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(mesh::VertexAttribute, tex_coord))
    .add(3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexAttribute, color));
    vkw::pipeline::VertexInputState frustum_vertex_input_state(frustum_vertex_bindings, frustum_vertex_attributes);
    vkw::pipeline::InputAssemblyState frustum_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState frustum_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState frustum_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState frustum_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState frustum_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates frustum_blend_attachment_states{};
    frustum_blend_attachment_states.add(VK_TRUE, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD);
    vkw::pipeline::ColorBlendState frustum_color_blend_state(frustum_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates frustum_pipeline_states{};
    frustum_pipeline_states
    .shader_stages(frustum_shader_stages)
    .vertex_input(frustum_vertex_input_state)
    .input_assembly(frustum_input_assembly_state)
    .viewport(frustum_viewport_state)
    .rasterization(frustum_rasterization_state)
    .multisample(frustum_multisample_state)
    .depth_stencil(frustum_depth_stencil_state)
    .color_blend(frustum_color_blend_state);

    auto frustum_pipeline = device->create_pipeline(frustum_pipeline_states, frustum_pipeline_layout, frustum_render_pass, 0).unwrap();

    // AABB transform shader
    vkw::pipeline::ComputeShaderStage aabb_trans_shader_stage{};
    aabb_trans_shader_stage.compute_shader(aabb_transform_shader);

    vkw::pipeline::ComputePipelineStates aabb_trans_pipeline_state(aabb_trans_shader_stage);

    auto aabb_trans_pipeline = device->create_pipeline(aabb_trans_pipeline_state, aabb_trans_pipeline_layout).unwrap();

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
    float frustum_rotate = 0.0f;

    vkw::barrier::BufferMemoryBarriers bunny_aabb_buffer_barrier{};
    bunny_aabb_buffer_barrier
    .add(
        bunny_aabb_instance_buffer, 0, sizeof(AABBInstanceData) * bunny_meshlet.size(),
        { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT }
    );

    vkw::barrier::BufferMemoryBarriers meshlet_aabb_buffer_barrier{};
    meshlet_aabb_buffer_barrier
    .add(
        meshlet_aabb_instance_buffer, 0, sizeof(AABBInstanceData) * bunny_meshlet.size(),
        { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT }
    );

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
            frustum_rotate += std::numbers::pi_v<float> * 0.5f * game->delta_time();

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

            glm::mat4 frustum_view = glm::lookAt(glm::vec3(0.0f), glm::vec3(glm::cos(frustum_rotate), 0.0f, glm::sin(frustum_rotate)), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 frustum_projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 5.0f);
            frustum_projection[1][1] *= -1;

            // why need to transpose?
            auto view_proj = glm::transpose(frustum_projection * frustum_view);
            auto row0 = view_proj[0];
            auto row1 = view_proj[1];
            auto row2 = view_proj[2];
            auto row3 = view_proj[3];

            auto inv_view_proj = glm::inverse(frustum_projection * frustum_view);

            ForwardConstantData push_constant_data1 {
                translate1 * rotate * model, view, projection,
            };

            ForwardConstantData push_constant_data2 {
                translate2 * rotate * model, view, projection,
            };

            FrustumConstantData frustum_constant_data {
                .view_proj = projection * view,
                .inv_view_proj = inv_view_proj,
            };

            command_buffer.begin_record(0)
            // forward pass
            .begin_render_pass(forward_framebuffers[current_image_index], forward_render_pass, render_area, clear_values)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, forward_pipeline)
            .push_constants(forward_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData), &push_constant_data1)
            .bind_vertex_buffers(0, {vertex_buffer})
            .bind_index_buffer(index_buffer, VK_INDEX_TYPE_UINT32)
            .draw_indexed(vkw::size_u32(bunny.indices().size()), 1)
            .push_constants(forward_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData), &push_constant_data2)
            .bind_vertex_buffers(0, {meshlet_vertex_buffer})
            .bind_index_buffer(meshlet_index_buffer, VK_INDEX_TYPE_UINT32)
            .draw_indexed(vkw::size_u32(meshlet.indices().size()), 1)
            .end_render_pass()
            // AABB transform pass
            .bind_pipeline(VK_PIPELINE_BIND_POINT_COMPUTE, aabb_trans_pipeline)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, aabb_trans_pipeline_layout, 0, {aabb_descriptor_sets[0].first})
            .push_constants(aabb_trans_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ForwardConstantData), &push_constant_data1)
            .dispatch(vkw::size_u32(bunny_meshlet.size()))
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, aabb_trans_pipeline_layout, 0, {aabb_descriptor_sets[1].first})
            .push_constants(aabb_trans_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ForwardConstantData), &push_constant_data2)
            .dispatch(vkw::size_u32(meshlet.meshlets().size()))
            // debug bounding box pass
            .pipeline_barrier({VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT}, 0, std::nullopt, bunny_aabb_buffer_barrier)
            .pipeline_barrier({VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT}, 0, std::nullopt, meshlet_aabb_buffer_barrier)
            .begin_render_pass(aabb_framebuffers[current_image_index], aabb_render_pass, render_area, {})
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, aabb_pipeline)
            .push_constants(aabb_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData), &push_constant_data1)
            .bind_vertex_buffers(0, {aabb_vertex_buffer, bunny_aabb_instance_buffer})
            .bind_index_buffer(aabb_index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed(vkw::size_u32(aabb_mesh.indices().size()), vkw::size_u32(bunny_aabbs.size()))
            .push_constants(aabb_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ForwardConstantData), &push_constant_data2)
            .bind_vertex_buffers(0, {aabb_vertex_buffer, meshlet_aabb_instance_buffer})
            .draw_indexed(vkw::size_u32(aabb_mesh.indices().size()), vkw::size_u32(meshlet_aabbs.size()))
            .end_render_pass()
            // debug frustum pass
            .begin_render_pass(frustum_framebuffers[current_image_index], frustum_render_pass, render_area, {})
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, frustum_pipeline)
            .push_constants(frustum_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FrustumConstantData), &frustum_constant_data)
            .bind_vertex_buffers(0, {frustum_vertex_buffer})
            .bind_index_buffer(frustum_index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed(vkw::size_u32(frustum_mesh.indices().size()), 1)
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