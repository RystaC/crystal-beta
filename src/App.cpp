#include "App.hpp"

bool App::init(int window_width, int window_height) {
    auto result = SDL_Init(SDL_INIT_VIDEO);
    if(result != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
        return false;
    }

    window_ = SDL_CreateWindow("crystal-beta demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_VULKAN);
    if(!window_) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL2] %s\n", SDL_GetError());
        return false;
    }

    return true;
}