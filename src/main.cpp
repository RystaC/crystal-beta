#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "App.hpp"

constexpr size_t WINDOW_WIDTH = 640;
constexpr size_t WINDOW_HEIGHT = 480;

int main(int, char**) {
    App app{};
    auto result = app.init(WINDOW_WIDTH, WINDOW_HEIGHT);
    if(!result) {
        std::cerr << "[crystal-beta] ERROR: failed to initialize application. exit." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    app.main_loop([](){});

    return 0;
}