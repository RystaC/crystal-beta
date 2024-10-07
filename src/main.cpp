#include <iostream>
#include <memory>
#include <vector>

#include <cmath>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game.hpp"
#include "vkw/Instance.hpp"
#include "vkw/Device.hpp"

#include "mesh/BasicMesh.hpp"

constexpr size_t WINDOW_WIDTH = 1280;
constexpr size_t WINDOW_HEIGHT = 720;

struct GeometryPushConstantData {
    glm::mat4 model, view, projection;
};

struct AABB {
    alignas(16) glm::vec3 min;
    alignas(16) glm::vec3 max;
};

struct CullingPushConstantData {
    glm::vec4 frustum_planes[6];
    glm::mat4 model;
    AABB box;
};

struct FrustumPushConstantData {
    glm::mat4 view_proj;
    glm::mat4 inv_view_proj;
};

constexpr uint32_t MAX_LIGHTS = 16;

struct LightsData {
    alignas(16) glm::vec3 ambient;
    alignas(16) struct {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
    } lights[MAX_LIGHTS];
};

struct InstanceBufferData {
    alignas(16) glm::vec3 translate;
};

int main(int argc, char** argv) {
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
    auto position_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT).unwrap();
    auto position_buffer_view = position_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();
    auto normal_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT).unwrap();
    auto normal_buffer_view = normal_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();
    auto albedo_buffer = device->create_image(swapchain.extent(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT).unwrap();
    auto albedo_buffer_view = albedo_buffer.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();

    auto blur_vertical_image = device->create_image(swapchain.extent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT).unwrap();
    auto blur_vertical_image_view = blur_vertical_image.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();
    auto blur_horizontal_image = device->create_image(swapchain.extent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT).unwrap();
    auto blur_horizontal_image_view = blur_horizontal_image.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();

    auto diffuse_image = device->create_image(swapchain.extent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT).unwrap();
    auto diffuse_image_view = diffuse_image.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();
    auto specular_image = device->create_image(swapchain.extent(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT).unwrap();
    auto specular_image_view = specular_image.create_image_view(VK_IMAGE_ASPECT_COLOR_BIT).unwrap();

    auto sampler = device->create_sampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE).unwrap();

    vkw::render_pass::AttachmentDescriptions deferred_attachment_descs{};
    deferred_attachment_descs
    // attachment 0: image for blur path (vertical)
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
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
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    )
    // attachment 3: g-buffer normal
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    )
    // attachment 4: g-buffer albedo
    .add(
        VK_FORMAT_R32G32B32A32_SFLOAT, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
    )
    // attachment 5: diffuse image
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
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
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    .add(5, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

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

    // vertical blur render pass
    vkw::render_pass::AttachmentDescriptions blur_vertical_attachment{};
    blur_vertical_attachment
    // attachment 0: output image (image for horizontal blur)
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
    );

    vkw::render_pass::AttachmentReferences blur_vertical_attachment_ref{};
    blur_vertical_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions blur_vertical_subpass{};
    blur_vertical_subpass.add()
    .output_color_attachments(blur_vertical_attachment_ref)
    .end();

    // horizontal blur render pass
    vkw::render_pass::AttachmentDescriptions blur_horizontal_attachment{};
    blur_horizontal_attachment
    // attachment 0: output image
    .add(
        VK_FORMAT_R8G8B8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
    );

    vkw::render_pass::AttachmentReferences blur_horizontal_attachment_ref{};
    blur_horizontal_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions blur_horizontal_subpass{};
    blur_horizontal_subpass.add()
    .output_color_attachments(blur_horizontal_attachment_ref)
    .end();

    // image merging render pass
    vkw::render_pass::AttachmentDescriptions merge_attachment{};
    merge_attachment
    // attachment 0: swapchain image
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    );

    vkw::render_pass::AttachmentReferences merge_attachment_ref{};
    merge_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions merge_subpass{};
    merge_subpass.add()
    .output_color_attachments(merge_attachment_ref)
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

    // debug bounding box render pass
    vkw::render_pass::AttachmentDescriptions bounding_box_attachment{};
    bounding_box_attachment
    .add(
        VK_FORMAT_B8G8R8A8_UNORM, VK_SAMPLE_COUNT_1_BIT,
        VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE,
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
    );

    vkw::render_pass::AttachmentReferences bounding_box_attachment_ref{};
    bounding_box_attachment_ref
    .add(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkw::render_pass::SubpassDescriptions bounding_box_subpass{};
    bounding_box_subpass.add()
    .output_color_attachments(bounding_box_attachment_ref)
    .end();

    std::cerr << std::endl << "create render pass..." << std::endl;
    auto deferred_render_pass = device->create_render_pass(deferred_attachment_descs, deferred_subpass_descs, deferred_pass_depends).unwrap();
    auto blur_vertical_render_pass = device->create_render_pass(blur_vertical_attachment, blur_vertical_subpass).unwrap();
    auto blur_horizontal_render_pass = device->create_render_pass(blur_horizontal_attachment, blur_horizontal_subpass).unwrap();
    auto merge_render_pass = device->create_render_pass(merge_attachment, merge_subpass).unwrap();
    auto frustum_render_pass = device->create_render_pass(frustum_attachment, frustum_subpass).unwrap();
    auto bounding_box_render_pass = device->create_render_pass(bounding_box_attachment, bounding_box_subpass).unwrap();

    std::cerr << std::endl << "create framebuffers..." << std::endl;
    auto deferred_framebuffer = device->create_framebuffer(
        deferred_render_pass,
        {
            blur_vertical_image_view, depth_buffer_view,
            position_buffer_view, normal_buffer_view, albedo_buffer_view,
            diffuse_image_view,
        },
        swapchain.extent()
    ).unwrap();

    auto blur_vertical_framebuffer = device->create_framebuffer(blur_vertical_render_pass, {blur_horizontal_image_view}, swapchain.extent()).unwrap();
    auto blur_horizontal_framebuffer = device->create_framebuffer(blur_horizontal_render_pass, {specular_image_view}, swapchain.extent()).unwrap();

    std::vector<vkw::object::Framebuffer> merge_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        merge_framebuffers[i] = device->create_framebuffer(merge_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    std::vector<vkw::object::Framebuffer> frustum_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        frustum_framebuffers[i] = device->create_framebuffer(frustum_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    std::vector<vkw::object::Framebuffer> bounding_box_framebuffers(swapchain.size());
    for(size_t i = 0; i < swapchain.size(); ++i) {
        bounding_box_framebuffers[i] = device->create_framebuffer(bounding_box_render_pass, {swapchain.image_view(i)}, swapchain.extent()).unwrap();
    }

    std::cerr << std::endl << "create shader modules..." << std::endl;
    auto geometry_vertex_shader = device->create_shader_module("shaders/deferred_geometry.vert.glsl.spirv").unwrap();
    auto geometry_fragment_shader = device->create_shader_module("shaders/deferred_geometry.frag.glsl.spirv").unwrap();
    auto plane_vertex_shader = device->create_shader_module("shaders/simple_plane.vert.glsl.spirv").unwrap();
    auto light_fragment_shader = device->create_shader_module("shaders/deferred_light.frag.glsl.spirv").unwrap();
    auto blur_fragment_shader = device->create_shader_module("shaders/gaussian_blur.frag.glsl.spirv").unwrap();
    auto merge_fragment_shader = device->create_shader_module("shaders/merge.frag.glsl.spirv").unwrap();
    auto frustum_vertex_shader = device->create_shader_module("shaders/debug_frustum.vert.glsl.spirv").unwrap();
    auto frustum_fragment_shader = device->create_shader_module("shaders/debug_frustum.frag.glsl.spirv").unwrap();
    auto bounding_box_vertex_shader = device->create_shader_module("shaders/debug_bounding_box.vert.glsl.spirv").unwrap();
    auto bounding_box_fragment_shader = device->create_shader_module("shaders/debug_bounding_box.frag.glsl.spirv").unwrap();
    auto instance_culling_shader = device->create_shader_module("shaders/frustum_culling_instance.comp.glsl.spirv").unwrap();

    // auto mesh = meshes::Mesh::torus(32, 32, 1.0f, 2.0f);
    auto mesh = mesh::BasicMesh::cube();

    // assume target mesh is same size cube. So no need to calculate bounding box.
    auto bounding_box_mesh = mesh::BasicMesh::frame();

    std::vector<InstanceBufferData> instance_data{};

    for(auto x = -8; x <= 8; x += 4) {
        for(auto y = -8; y <= 8; y += 4) {
            for(auto z = -8; z <= 8; z += 4) {
                instance_data.emplace_back(
                    InstanceBufferData {
                        glm::vec3((float)x, (float)y, (float)z),
                    }
                );
            }
        }
    }

    std::vector<LightsData> light_uniform_buffer_data = {
        LightsData {
            .ambient = glm::vec3(0.2f),
            .lights = {
                { .position = glm::vec3(0.0f, 0.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(15.0f, 0.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, 15.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(15.0f, 15.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, 0.0f, 15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(15.0f, 0.0f, 15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, 15.0f, 15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(15.0f, 15.0f, 15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(-15.0f, 0.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, -15.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(-15.0f, -15.0f, 0.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, 0.0f, -15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(-15.0f, 0.0f, -15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(0.0f, -15.0f, -15.0f), .color = glm::vec3(0.2f) },
                { .position = glm::vec3(-15.0f, -15.0f, -15.0f), .color = glm::vec3(0.2f) },
            },
        }
    };

    std::cerr << std::endl << "create buffers..." << std::endl;
    auto vertex_buffer = device->create_buffer_with_data(mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto index_buffer = device->create_buffer_with_data(mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();
    auto light_uniform_buffer = device->create_buffer_with_data(light_uniform_buffer_data, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT).unwrap();
    auto bounding_box_vertex_buffer = device->create_buffer_with_data(bounding_box_mesh.vertices(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    auto bounding_box_index_buffer = device->create_buffer_with_data(bounding_box_mesh.indices(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT).unwrap();

    // indirect buffer for frustum culling
    auto instance_in_buffer = device->create_buffer_with_data(instance_data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();
    std::vector<InstanceBufferData> instance_data_empty(instance_data.size());
    auto instance_out_buffer = device->create_buffer_with_data(instance_data_empty, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT).unwrap();

    std::vector<VkDrawIndexedIndirectCommand> indirect_data = {
        { vkw::size_u32(mesh.indices().size()), 0, 0, 0, 0 },
    };
    auto indirect_buffer = device->create_buffer_with_data(indirect_data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT).unwrap();

    // deferred path set 0: g-buffer inputs
    vkw::descriptor::DescriptorSetLayoutBindings light_attachment_layout_bindings(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
    light_attachment_layout_bindings
    // g-buffer: position
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
    // g-buffer: normal
    .add(1, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
    // g-buffer: albedo
    .add(2, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // deferred path set 1: lights uniform buffer
    vkw::descriptor::DescriptorSetLayoutBindings light_uniform_layout_bindings(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    light_uniform_layout_bindings
    // lights
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // vertical blur path set 0: input image of scene
    vkw::descriptor::DescriptorSetLayoutBindings blur_vertical_layout_bindings(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    blur_vertical_layout_bindings
    // input texture
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // horizontal blur path set 0: input image of vertical blurred image
    vkw::descriptor::DescriptorSetLayoutBindings blur_horizontal_layout_bindings(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    blur_horizontal_layout_bindings
    // input texture
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // merge path set 0: input images: diffuse image and blurred spacular image
    vkw::descriptor::DescriptorSetLayoutBindings merge_layout_bindings(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    merge_layout_bindings
    // diffuse texture
    .add(0, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
    // specular texture
    .add(1, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // frustum culling set 0: storage buffer
    vkw::descriptor::DescriptorSetLayoutBindings instance_culling_layout_bindings(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    instance_culling_layout_bindings
    // input instance data
    .add(0, 1, VK_SHADER_STAGE_COMPUTE_BIT)
    // output instance data
    .add(1, 1, VK_SHADER_STAGE_COMPUTE_BIT)
    // indirect buffer
    .add(2, 1, VK_SHADER_STAGE_COMPUTE_BIT);

    std::cerr << std::endl << "create descriptor set layout..." << std::endl;
    auto light_attachment_descriptor_layout = device->create_descriptor_set_layout(light_attachment_layout_bindings).unwrap();
    auto light_uniform_descriptor_layout = device->create_descriptor_set_layout(light_uniform_layout_bindings).unwrap();
    auto blur_vertical_descriptor_layout = device->create_descriptor_set_layout(blur_vertical_layout_bindings).unwrap();
    auto blur_horizontal_descriptor_layout = device->create_descriptor_set_layout(blur_horizontal_layout_bindings).unwrap();
    auto merge_descriptor_layout = device->create_descriptor_set_layout(merge_layout_bindings).unwrap();
    auto instance_culling_descriptor_layout = device->create_descriptor_set_layout(instance_culling_layout_bindings).unwrap();

    std::cerr << std::endl << "create descriptor pool..." << std::endl;
    auto light_descriptor_pool = device->create_descriptor_pool({light_attachment_layout_bindings, light_uniform_layout_bindings}).unwrap();
    auto blur_vertical_descriptor_pool = device->create_descriptor_pool({blur_vertical_layout_bindings}).unwrap();
    auto blur_horizontal_descriptor_pool = device->create_descriptor_pool({blur_horizontal_layout_bindings}).unwrap();
    auto merge_descriptor_pool = device->create_descriptor_pool({merge_layout_bindings}).unwrap();
    auto instance_culling_descriptor_pool = device->create_descriptor_pool({instance_culling_layout_bindings}).unwrap();

    std::cerr << std::endl << "allocate descriptor sets..." << std::endl;
    auto light_descriptor_sets = light_descriptor_pool.allocate_descriptor_sets({light_attachment_descriptor_layout, light_uniform_descriptor_layout}).unwrap();
    auto blur_vertical_descriptor_sets = blur_vertical_descriptor_pool.allocate_descriptor_sets({blur_vertical_descriptor_layout}).unwrap();
    auto blur_horizontal_descriptor_sets = blur_horizontal_descriptor_pool.allocate_descriptor_sets({blur_horizontal_descriptor_layout}).unwrap();
    auto merge_descriptor_sets = merge_descriptor_pool.allocate_descriptor_sets({merge_descriptor_layout}).unwrap();
    auto instance_culling_descriptor_sets = instance_culling_descriptor_pool.allocate_descriptor_sets({instance_culling_descriptor_layout}).unwrap();

    vkw::descriptor::ImageInfos light_image_infos{};
    light_image_infos
    .add(position_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(normal_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    .add(albedo_buffer_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkw::descriptor::BufferInfos light_buffer_infos{};
    light_buffer_infos
    .add(light_uniform_buffer, 0, sizeof(LightsData));
    vkw::descriptor::WriteDescriptorSets light_writes{};
    light_writes
    .add(light_descriptor_sets[0], 0, 0, light_image_infos)
    .add(light_descriptor_sets[1], 0, 0, light_buffer_infos);

    vkw::descriptor::ImageInfos blur_vertical_image_info{};
    blur_vertical_image_info
    .add(blur_vertical_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler);
    vkw::descriptor::WriteDescriptorSets blur_vertical_writes{};
    blur_vertical_writes
    .add(blur_vertical_descriptor_sets[0], 0, 0, blur_vertical_image_info);

    vkw::descriptor::ImageInfos blur_horizontal_image_info{};
    blur_horizontal_image_info
    .add(blur_horizontal_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler);
    vkw::descriptor::WriteDescriptorSets blur_horizontal_writes{};
    blur_horizontal_writes
    .add(blur_horizontal_descriptor_sets[0], 0, 0, blur_horizontal_image_info);

    vkw::descriptor::ImageInfos merge_image_infos{};
    merge_image_infos
    .add(diffuse_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler)
    .add(specular_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, sampler);
    vkw::descriptor::WriteDescriptorSets merge_writes{};
    merge_writes
    .add(merge_descriptor_sets[0], 0, 0, merge_image_infos);

    vkw::descriptor::BufferInfos instance_culling_buffer_infos{};
    instance_culling_buffer_infos
    .add(instance_in_buffer, 0, sizeof(InstanceBufferData) * instance_data.size())
    .add(instance_out_buffer, 0, sizeof(InstanceBufferData) * instance_data.size())
    .add(indirect_buffer, 0, sizeof(VkDrawIndexedIndirectCommand));
    vkw::descriptor::WriteDescriptorSets instance_culling_writes{};
    instance_culling_writes
    .add(instance_culling_descriptor_sets[0], 0, 0, instance_culling_buffer_infos);

    std::cerr << std::endl << "update descriptor sets..." << std::endl;
    device->update_descriptor_sets(light_writes);
    device->update_descriptor_sets(blur_vertical_writes);
    device->update_descriptor_sets(blur_horizontal_writes);
    device->update_descriptor_sets(merge_writes);
    device->update_descriptor_sets(instance_culling_writes);

    vkw::pipeline_layout::CreateInfo geometry_layout_info{};
    geometry_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GeometryPushConstantData));

    vkw::pipeline_layout::CreateInfo light_layout_info{};
    light_layout_info
    .add_descriptor_set_layout(light_attachment_descriptor_layout)
    .add_descriptor_set_layout(light_uniform_descriptor_layout)
    .add_push_constant_range(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3));

    vkw::pipeline_layout::CreateInfo blur_vertical_layout_info{};
    blur_vertical_layout_info
    .add_descriptor_set_layout(blur_vertical_descriptor_layout);

    vkw::pipeline_layout::CreateInfo blur_horizontal_layout_info{};
    blur_horizontal_layout_info
    .add_descriptor_set_layout(blur_horizontal_descriptor_layout);

    vkw::pipeline_layout::CreateInfo merge_layout_info{};
    merge_layout_info
    .add_descriptor_set_layout(merge_descriptor_layout);

    vkw::pipeline_layout::CreateInfo frustum_layout_info{};
    frustum_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FrustumPushConstantData));

    vkw::pipeline_layout::CreateInfo bounding_box_layout_info{};
    bounding_box_layout_info
    .add_push_constant_range(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GeometryPushConstantData));

    vkw::pipeline_layout::CreateInfo instance_culling_layout_info{};
    instance_culling_layout_info
    .add_descriptor_set_layout(instance_culling_descriptor_layout)
    .add_push_constant_range(VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(CullingPushConstantData));

    std::cerr << std::endl << "create pipeline layout..." << std::endl;
    auto geometry_pipeline_layout = device->create_pipeline_layout(geometry_layout_info).unwrap();
    auto light_pipeline_layout = device->create_pipeline_layout(light_layout_info).unwrap();
    auto blur_vertical_pipeline_layout = device->create_pipeline_layout(blur_vertical_layout_info).unwrap();
    auto blur_horizontal_pipeline_layout = device->create_pipeline_layout(blur_horizontal_layout_info).unwrap();
    auto merge_pipeline_layout = device->create_pipeline_layout(merge_layout_info).unwrap();
    auto frustum_pipeline_layout = device->create_pipeline_layout(frustum_layout_info).unwrap();
    auto bounding_box_pipeline_layout = device->create_pipeline_layout(bounding_box_layout_info).unwrap();
    auto instance_culling_pipeline_layout = device->create_pipeline_layout(instance_culling_layout_info).unwrap();

    std::cerr << std::endl << "create pipelines..." << std::endl;

    vkw::pipeline::GraphicsShaderStages geometry_shader_stages{};
    geometry_shader_stages
    .vertex_shader(geometry_vertex_shader)
    .fragment_shader(geometry_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions geometry_input_bindings{};
    geometry_input_bindings
    .add(0, sizeof(mesh::VertexData), VK_VERTEX_INPUT_RATE_VERTEX)
    .add(1, sizeof(InstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE);
    vkw::pipeline::VertexInputAttributeDescriptions geometry_input_attributes{};
    geometry_input_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, normal))
    .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexData, color))
    .add(3, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceBufferData, translate));
    vkw::pipeline::VertexInputState geometry_vertex_input_state(geometry_input_bindings, geometry_input_attributes);
    vkw::pipeline::InputAssemblyState geometry_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState geometry_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState geometry_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState geometry_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState geometry_depth_stencil_state(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates geometry_blend_attachment_states{};
    geometry_blend_attachment_states
    .add()
    .add()
    .add();
    vkw::pipeline::ColorBlendState geometory_color_blend_state(geometry_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates geometry_pipeline_state{};
    geometry_pipeline_state
    .shader_stages(geometry_shader_stages)
    .vertex_input(geometry_vertex_input_state)
    .input_assembly(geometry_input_assembly_state)
    .viewport(geometry_viewport_state)
    .rasterization(geometry_rasterization_state)
    .multisample(geometry_multisample_state)
    .depth_stencil(geometry_depth_stencil_state)
    .color_blend(geometory_color_blend_state);


    auto geometry_pipeline = device->create_pipeline(geometry_pipeline_state, geometry_pipeline_layout, deferred_render_pass, 0).unwrap();

    vkw::pipeline::GraphicsShaderStages light_shader_stages{};
    light_shader_stages
    .vertex_shader(plane_vertex_shader)
    .fragment_shader(light_fragment_shader);
    vkw::pipeline::VertexInputState light_vertex_input_state{};
    vkw::pipeline::InputAssemblyState light_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState light_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState light_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState light_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState light_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates light_blend_attachment_states{};
    light_blend_attachment_states
    .add()
    .add();
    vkw::pipeline::ColorBlendState light_color_blend_state(light_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates light_pipeline_states{};
    light_pipeline_states
    .shader_stages(light_shader_stages)
    .vertex_input(light_vertex_input_state)
    .input_assembly(light_input_assembly_state)
    .viewport(light_viewport_state)
    .rasterization(light_rasterization_state)
    .multisample(light_multisample_state)
    .depth_stencil(light_depth_stencil_state)
    .color_blend(light_color_blend_state);

    auto light_pipeline = device->create_pipeline(light_pipeline_states, light_pipeline_layout, deferred_render_pass, 1).unwrap();

    std::vector<VkBool32> blur_vertical_constant_data = { VK_FALSE };
    std::vector<VkBool32> blur_horizontal_constant_data = { VK_TRUE };

    vkw::pipeline::SpecializationMapEntries blur_constant_entry{};
    blur_constant_entry
    .add_entry(0, 0, sizeof(VkBool32));
    vkw::pipeline::SpecializationConstants blur_vertical_constant(blur_constant_entry, blur_vertical_constant_data);
    vkw::pipeline::SpecializationConstants blur_horizontal_constant(blur_constant_entry, blur_horizontal_constant_data);

    // blur vertical pipeline states
    vkw::pipeline::GraphicsShaderStages blur_vertical_shader_stages{};
    blur_vertical_shader_stages
    .vertex_shader(plane_vertex_shader)
    .fragment_shader(blur_fragment_shader, blur_vertical_constant);
    vkw::pipeline::VertexInputState blur_vertical_vertex_input_state{};
    vkw::pipeline::InputAssemblyState blur_vertical_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState blur_vertical_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState blur_vertical_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState blur_vertical_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState blur_vertical_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates blur_vertical_blend_attachment_states{};
    blur_vertical_blend_attachment_states.add();
    vkw::pipeline::ColorBlendState blur_vertical_color_blend_state(blur_vertical_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates blur_vertical_pipeline_states{};
    blur_vertical_pipeline_states
    .shader_stages(blur_vertical_shader_stages)
    .vertex_input(blur_vertical_vertex_input_state)
    .input_assembly(blur_vertical_input_assembly_state)
    .viewport(blur_vertical_viewport_state)
    .rasterization(blur_vertical_rasterization_state)
    .multisample(blur_vertical_multisample_state)
    .depth_stencil(blur_vertical_depth_stencil_state)
    .color_blend(blur_vertical_color_blend_state);

    auto blur_vertical_pipeline = device->create_pipeline(blur_vertical_pipeline_states, blur_vertical_pipeline_layout, blur_vertical_render_pass, 0).unwrap();

    // blur horizontal pipeline states
    vkw::pipeline::GraphicsShaderStages blur_horizontal_shader_stages{};
    blur_horizontal_shader_stages
    .vertex_shader(plane_vertex_shader)
    .fragment_shader(blur_fragment_shader, blur_horizontal_constant);
    vkw::pipeline::VertexInputState blur_horizontal_vertex_input_state{};
    vkw::pipeline::InputAssemblyState blur_horizontal_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState blur_horizontal_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState blur_horizontal_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState blur_horizontal_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState blur_horizontal_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates blur_horizontal_blend_attachment_states{};
    blur_horizontal_blend_attachment_states.add();
    vkw::pipeline::ColorBlendState blur_horizontal_color_blend_state(blur_horizontal_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates blur_horizontal_pipeline_states{};
    blur_horizontal_pipeline_states
    .shader_stages(blur_horizontal_shader_stages)
    .vertex_input(blur_horizontal_vertex_input_state)
    .input_assembly(blur_horizontal_input_assembly_state)
    .viewport(blur_horizontal_viewport_state)
    .rasterization(blur_horizontal_rasterization_state)
    .multisample(blur_horizontal_multisample_state)
    .depth_stencil(blur_horizontal_depth_stencil_state)
    .color_blend(blur_horizontal_color_blend_state);

    auto blur_horizontal_pipeline = device->create_pipeline(blur_horizontal_pipeline_states, blur_horizontal_pipeline_layout, blur_horizontal_render_pass, 0).unwrap();

    // merge pipeline states
    vkw::pipeline::GraphicsShaderStages merge_shader_stages{};
    merge_shader_stages
    .vertex_shader(plane_vertex_shader)
    .fragment_shader(merge_fragment_shader);
    vkw::pipeline::VertexInputState merge_vertex_input_state{};
    vkw::pipeline::InputAssemblyState merge_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkw::pipeline::ViewportState merge_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState merge_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState merge_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState merge_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates merge_blend_attachment_states{};
    merge_blend_attachment_states.add();
    vkw::pipeline::ColorBlendState merge_color_blend_state(merge_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates merge_pipeline_states{};
    merge_pipeline_states
    .shader_stages(merge_shader_stages)
    .vertex_input(merge_vertex_input_state)
    .input_assembly(merge_input_assembly_state)
    .viewport(merge_viewport_state)
    .rasterization(merge_rasterization_state)
    .multisample(merge_multisample_state)
    .depth_stencil(merge_depth_stencil_state)
    .color_blend(merge_color_blend_state);

    auto merge_pipeline = device->create_pipeline(merge_pipeline_states, merge_pipeline_layout, merge_render_pass, 0).unwrap();

    // debug frustum pipeline
    vkw::pipeline::GraphicsShaderStages frustum_shader_stages{};
    frustum_shader_stages
    .vertex_shader(frustum_vertex_shader)
    .fragment_shader(frustum_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions frustum_vertex_bindings{};
    frustum_vertex_bindings
    .add(0, sizeof(mesh::VertexData), VK_VERTEX_INPUT_RATE_VERTEX);
    vkw::pipeline::VertexInputAttributeDescriptions frustum_vertex_attributes{};
    frustum_vertex_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, normal))
    .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexData, color));
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

    // debug bounding box pipeline
    vkw::pipeline::GraphicsShaderStages bounding_box_shader_stages{};
    bounding_box_shader_stages
    .vertex_shader(bounding_box_vertex_shader)
    .fragment_shader(bounding_box_fragment_shader);
    vkw::pipeline::VertexInputBindingDescriptions bounding_box_vertex_bindings{};
    bounding_box_vertex_bindings
    .add(0, sizeof(mesh::VertexData), VK_VERTEX_INPUT_RATE_VERTEX)
    .add(1, sizeof(InstanceBufferData), VK_VERTEX_INPUT_RATE_INSTANCE);
    vkw::pipeline::VertexInputAttributeDescriptions bounding_box_vertex_attributes{};
    bounding_box_vertex_attributes
    .add(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, position))
    .add(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(mesh::VertexData, normal))
    .add(2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(mesh::VertexData, color))
    .add(3, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceBufferData, translate));
    vkw::pipeline::VertexInputState bounding_box_vertex_input_state(bounding_box_vertex_bindings, bounding_box_vertex_attributes);
    vkw::pipeline::InputAssemblyState bounding_box_input_assembly_state(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
    vkw::pipeline::ViewportState bounding_box_viewport_state(
        {
            .x = 0.0f, .y = 0.0f,
            .width = static_cast<float>(WINDOW_WIDTH), .height = static_cast<float>(WINDOW_HEIGHT),
            .minDepth = 0.0f, .maxDepth = 1.0f
        },
        {.offset = {0, 0}, .extent = {WINDOW_WIDTH, WINDOW_HEIGHT}}
    );
    vkw::pipeline::RasterizarionState bounding_box_rasterization_state(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 1.0f);
    vkw::pipeline::MultisampleState bounding_box_multisample_state(VK_SAMPLE_COUNT_1_BIT);
    vkw::pipeline::DepthStencilState bounding_box_depth_stencil_state(VK_FALSE, VK_FALSE, VK_COMPARE_OP_NEVER, VK_FALSE, VK_FALSE);
    vkw::pipeline::ColorBlendAttachmentStates bounding_box_blend_attachment_states{};
    bounding_box_blend_attachment_states.add();
    vkw::pipeline::ColorBlendState bounding_box_color_blend_state(bounding_box_blend_attachment_states);

    vkw::pipeline::GraphicsPipelineStates bounding_box_pipeline_states{};
    bounding_box_pipeline_states
    .shader_stages(bounding_box_shader_stages)
    .vertex_input(bounding_box_vertex_input_state)
    .input_assembly(bounding_box_input_assembly_state)
    .viewport(bounding_box_viewport_state)
    .rasterization(bounding_box_rasterization_state)
    .multisample(bounding_box_multisample_state)
    .depth_stencil(bounding_box_depth_stencil_state)
    .color_blend(bounding_box_color_blend_state);

    auto bounding_box_pipeline = device->create_pipeline(bounding_box_pipeline_states, bounding_box_pipeline_layout, bounding_box_render_pass, 0).unwrap();

    vkw::pipeline::ComputeShaderStage instance_culling_shader_stage{};
    instance_culling_shader_stage
    .compute_shader(instance_culling_shader);

    vkw::pipeline::ComputePipelineStates instance_culling_pipeline_states(instance_culling_shader_stage);

    auto instance_culling_pipeline = device->create_pipeline(instance_culling_pipeline_states, instance_culling_pipeline_layout).unwrap();

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

    game->main_loop(
        [&]() {
            VkRect2D render_area = {{0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT}};
            std::vector<VkClearValue> clear_values = {
                // 0: vertical blur image
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                // 1: depth buffer
                { .depthStencil = {1.0f, 0} },
                // 2: g-buffer position
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                // 3: g-buffer normal
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                // 4: g-buffer albedo
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
                // 5: diffuse image
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
            };

            std::vector<VkClearValue> blur_clear_value = {
                { .color = {0.0f, 0.0f, 0.0f, 1.0f} },
            };

            model_rotate += std::numbers::pi_v<float> * game->delta_time();
            frustum_rotate += std::numbers::pi_v<float> * 0.5f * game->delta_time();

            glm::mat4 model = glm::rotate(glm::mat4(1.0f), model_rotate, glm::vec3(1.0f));
            auto camera_pos = game->camera_pos();
            auto camera_dir = game->camera_dir();
            auto camera_up = game->camera_up();
            glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_dir, camera_up);
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
            projection[1][1] *= -1;

            glm::mat4 frustum_view = glm::lookAt(glm::vec3(0.0f), glm::vec3(glm::cos(frustum_rotate), 0.0f, glm::sin(frustum_rotate)), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 frustum_projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 10.0f);
            frustum_projection[1][1] *= -1;

            // why need to transpose?
            auto view_proj = glm::transpose(frustum_projection * frustum_view);
            auto row0 = view_proj[0];
            auto row1 = view_proj[1];
            auto row2 = view_proj[2];
            auto row3 = view_proj[3];

            auto inv_view_proj = glm::inverse(frustum_projection * frustum_view);

            CullingPushConstantData instance_culling_constant_data {
                .frustum_planes = {
                    row3 + row0, // left
                    row3 - row0, // right
                    row3 + row1, // bottom
                    row3 - row1, // top
                    row3 + row2, // near
                    row3 - row2, // far
                },
                .model = model,
                .box = { glm::vec3(-1.0f), glm::vec3(1.0f) },
            };

            GeometryPushConstantData geometry_constant_data {
                model, view, projection,
            };

            FrustumPushConstantData frustum_constant_data {
                .view_proj = projection * view,
                .inv_view_proj = inv_view_proj,
            };

            vkw::barrier::BufferMemoryBarriers instance_culling_barrier{};
            instance_culling_barrier
            .add(
                instance_out_buffer, 0, sizeof(InstanceBufferData) * instance_data.size(),
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT }
            )
            .add(
                indirect_buffer, 0, sizeof(VkDrawIndexedIndirectCommand),
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT }
            );

            vkw::barrier::ImageMemoryBarriers blur_vertical_barrier{};
            blur_vertical_barrier
            .add(
                blur_vertical_image, {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT },
                { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
            );

            vkw::barrier::ImageMemoryBarriers blur_horizontal_barrier{};
            blur_horizontal_barrier
            .add(
                blur_horizontal_image, {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT },
                { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
            );

            vkw::barrier::ImageMemoryBarriers merge_barrier{};
            merge_barrier
            .add(
                diffuse_image, {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT },
                { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
            )
            .add(
                specular_image, {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1, },
                { VK_ACCESS_MEMORY_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT },
                { VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
            );

            command_buffer.begin_record(0)
            // instance culling path
            .bind_pipeline(VK_PIPELINE_BIND_POINT_COMPUTE, instance_culling_pipeline)
            .push_constants(instance_culling_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(CullingPushConstantData), &instance_culling_constant_data)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_COMPUTE, instance_culling_pipeline_layout, 0, instance_culling_descriptor_sets.sets())
            .dispatch(vkw::size_u32(instance_data.size()))
            // need barrier
            .pipeline_barrier(
                { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT }, 0,
                std::nullopt, instance_culling_barrier, std::nullopt
            )
            // deferred geometry path
            .begin_render_pass(deferred_framebuffer, deferred_render_pass, render_area, clear_values)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, geometry_pipeline)
            .push_constants(geometry_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GeometryPushConstantData), &geometry_constant_data)
            .bind_vertex_buffers(0, {vertex_buffer, instance_out_buffer})
            .bind_index_buffer(index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed_indirect(indirect_buffer, 0, 1, sizeof(VkDrawIndexedIndirectCommand))
            // deferred light path
            .next_subpass()
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, light_pipeline)
            .push_constants(light_pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &camera_pos)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, light_pipeline_layout, 0, light_descriptor_sets.sets())
            .draw(3, 1)
            .end_render_pass()
            // need barrier
            .pipeline_barrier(
                { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT }, 0,
                std::nullopt, std::nullopt, blur_vertical_barrier
            )
            // blur vertical path
            .begin_render_pass(blur_vertical_framebuffer, blur_vertical_render_pass, render_area, blur_clear_value)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, blur_vertical_pipeline)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, blur_vertical_pipeline_layout, 0, blur_vertical_descriptor_sets.sets())
            .draw(3, 1)
            .end_render_pass()
            // need barrier
            .pipeline_barrier(
                { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT }, 0,
                std::nullopt, std::nullopt, blur_horizontal_barrier
            )
            // blur horizontal path
            .begin_render_pass(blur_horizontal_framebuffer, blur_horizontal_render_pass, render_area, blur_clear_value)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, blur_horizontal_pipeline)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, blur_horizontal_pipeline_layout, 0, blur_horizontal_descriptor_sets.sets())
            .draw(3, 1)
            .end_render_pass()
            // need barrier
            .pipeline_barrier(
                { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT }, VK_DEPENDENCY_BY_REGION_BIT,
                std::nullopt, std::nullopt, merge_barrier
            )
            // merge path
            .begin_render_pass(merge_framebuffers[current_image_index], merge_render_pass, render_area, blur_clear_value)
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, merge_pipeline)
            .bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, merge_pipeline_layout, 0, merge_descriptor_sets.sets())
            .draw(3, 1)
            .end_render_pass()
            // debug frustum pass
            .begin_render_pass(frustum_framebuffers[current_image_index], frustum_render_pass, render_area, {})
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, frustum_pipeline)
            .push_constants(frustum_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FrustumPushConstantData), &frustum_constant_data)
            .bind_vertex_buffers(0, {vertex_buffer})
            .bind_index_buffer(index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed(vkw::size_u32(mesh.indices().size()), 1)
            .end_render_pass()
            // debug bounding box pass
            .begin_render_pass(bounding_box_framebuffers[current_image_index], bounding_box_render_pass, render_area, {})
            .bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, bounding_box_pipeline)
            .push_constants(bounding_box_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GeometryPushConstantData), &geometry_constant_data)
            .bind_vertex_buffers(0, {bounding_box_vertex_buffer, instance_in_buffer})
            .bind_index_buffer(bounding_box_index_buffer, VK_INDEX_TYPE_UINT16)
            .draw_indexed(vkw::size_u32(bounding_box_mesh.indices().size()), vkw::size_u32(instance_data.size()))
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