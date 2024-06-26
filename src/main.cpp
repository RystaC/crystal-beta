#include <iostream>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "App.hpp"
#include "vkw/Instance.hpp"
#include "vkw/Device.hpp"

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 480;

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

    for(const auto& pd : physical_device) std::cout << "physical device: " << pd.name() << std::endl;

    // auto device = std::make_unique<vkw::Device>(physical_device);
    // {
    //     auto queue_family = physical_device->find_queue_family(VK_QUEUE_GRAPHICS_BIT);
    //     if(queue_family < 0) {
    //         std::cerr << "[crystal-beta] ERROR: failed to find queue family. exit." << std::endl;
    //         std::exit(EXIT_FAILURE);
    //     }

    //     std::vector<vkw::QueueTarget> targets {
    //         { static_cast<uint32_t>(queue_family), { 0.0f } },
    //     };
    //     auto extensions = std::vector<const char*>{ "VK_KHR_swapchain" };
    //     auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

    //     auto res = device->init(targets, extensions, layers);
    //     if(!res) {
    //         std::cerr << "[crystal-beta] ERROR: failed to initialize Vulkan device. exit." << std::endl;
    //         std::exit(EXIT_FAILURE);
    //     }
    // }

    // auto command_pool = device->create_command_pool();
    // if(!command_pool) {
    //     std::cerr << "[crystal-beta] ERROR: failed to create command pool. exit." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    // auto swapchain = device->create_swapchain(*surface, {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}, VK_PRESENT_MODE_FIFO_KHR, WINDOW_WIDTH, WINDOW_HEIGHT);
    // if(!swapchain) {
    //     std::cerr << "[crystal-beta] ERROR: failed to create swapchain. exit." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    app->main_loop([](){});

    return 0;
}