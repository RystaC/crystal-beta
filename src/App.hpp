#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

class App {
    SDL_Window* window_;
    SDL_Event event_;
    uint64_t ticks_;
    bool quit_;

public:
    App() noexcept : window_(nullptr), event_(), ticks_(0ull), quit_(false) {}
    ~App() noexcept {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    auto window() const noexcept { return window_; }
    auto ticks() const noexcept { return ticks_; }

    bool init(int window_width, int window_height);

    std::vector<const char*> enum_extensions();

    template<typename F>
    void main_loop(F func) {
        while(!quit_) {
            SDL_SetWindowTitle(window_, std::to_string(ticks_).c_str());

            func();

            while(SDL_PollEvent(&event_)) {
                if(event_.type == SDL_QUIT) quit_ = true;
                else if(event_.type == SDL_KEYDOWN && event_.key.keysym.sym == SDLK_ESCAPE) quit_ = true;
            }

            ++ticks_;
        }
    }
};