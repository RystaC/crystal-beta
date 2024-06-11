#include <iostream>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "App.hpp"
#include "vkw/Instance.hpp"

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 480;

int main(int, char**) {
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
    std::cout << std::boolalpha << bool(surface) << std::endl;

    app->main_loop([](){});

    return 0;
}