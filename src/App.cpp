#include "App.hpp"

bool App::init(int window_width, int window_height) {
    auto result = SDL_Init(SDL_INIT_VIDEO);
    if(result != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_VULKAN);
    if(!window_) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
        return false;
    }

    window_width_ = window_width;
    window_height_ = window_height;

    SDL_ShowCursor(SDL_DISABLE);

    return true;
}

std::vector<const char*> App::enum_extensions() {
    uint32_t extension_count{};
    auto result = SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, nullptr);
    if(result != SDL_TRUE) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
        return {};
    }

    std::vector<const char*> extensions(extension_count);
    result = SDL_Vulkan_GetInstanceExtensions(window_, &extension_count, extensions.data());
    if(result != SDL_TRUE) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
    }

    return extensions;
}