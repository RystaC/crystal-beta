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
    int window_width_, window_height_;
    SDL_Event event_;
    uint64_t ticks_;
    bool quit_;

    // states
    int mouse_x_, mouse_y_;
    uint32_t mouse_buttons_;

public:
    App() noexcept : window_(nullptr), ticks_(0llu), quit_(false) {}
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
            // get mouse cursor position
            mouse_buttons_ = SDL_GetMouseState(&mouse_x_, &mouse_y_);
            mouse_x_ = mouse_x_ - window_width_ / 2;
            mouse_y_ = window_height_ / 2 - mouse_y_;
            bool mouse_left = mouse_buttons_ & SDL_BUTTON(SDL_BUTTON_LEFT);
            bool mouse_middle = mouse_buttons_ & SDL_BUTTON(SDL_BUTTON_MIDDLE);
            bool mouse_right = mouse_buttons_ & SDL_BUTTON(SDL_BUTTON_RIGHT);
            bool mouse_x1 = mouse_buttons_ & SDL_BUTTON(SDL_BUTTON_X1);
            bool mouse_x2 = mouse_buttons_ & SDL_BUTTON(SDL_BUTTON_X2);

            // rendering process
            func();

            while(SDL_PollEvent(&event_)) {
                if(event_.type == SDL_QUIT) quit_ = true;
                else if(event_.type == SDL_KEYDOWN && event_.key.keysym.sym == SDLK_ESCAPE) quit_ = true;
            }

            auto current_time_ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

            ++ticks_;

            // calculate frame rate
            auto fps = (double)ticks_ / (current_time_ms - previous_time_ms) * 1000.0;
            char m_l = mouse_left ? 'l' : ' ';
            char m_m = mouse_middle ? 'm' : ' ';
            char m_r = mouse_right ? 'r' : ' ';
            char m_x1 = mouse_x1 ? '1' : ' ';
            char m_x2 = mouse_x2 ? '2' : ' ';

            SDL_SetWindowTitle(window_, std::format("fps = {:.2f}, ticks {}, mouse pos = ({}, {}), mouse buttons = [{}{}{}{}{}]", fps, ticks_, mouse_x_, mouse_y_, m_l, m_m, m_r, m_x1, m_x2).c_str());
        }
    }
};