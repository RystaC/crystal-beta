#pragma once

#include <chrono>
#include <format>
#include <string>
#include <thread>
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
        using namespace std::chrono;

        auto previous_time_ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        while(!quit_) {
            func();

            while(SDL_PollEvent(&event_)) {
                if(event_.type == SDL_QUIT) quit_ = true;
                else if(event_.type == SDL_KEYDOWN && event_.key.keysym.sym == SDLK_ESCAPE) quit_ = true;
            }

            auto current_time_ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            ++ticks_;

            auto fps = (double)ticks_ / (current_time_ms - previous_time_ms) * 1000.0;

            SDL_SetWindowTitle(window_, std::format("[crystal-beta demo] fps = {:.2f}, ticks {}", fps, ticks_).c_str());
        }
    }
};