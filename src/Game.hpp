#pragma once

#include <array>
#include <chrono>
#include <format>
#include <numbers>
#include <string>
#include <thread>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <glm/glm.hpp>

class Game {
    SDL_Window* window_;
    int window_width_, window_height_;
    SDL_Event event_;
    uint64_t ticks_;
    float delta_time_;
    bool quit_;

    // states
    // mouse

    bool is_mouse_relative_;
    int mouse_x_, mouse_y_;
    uint32_t mouse_buttons_;

    // keyboard

    std::array<uint8_t, SDL_NUM_SCANCODES> prev_key_states_;
    const uint8_t* curr_key_states_;
    enum class KeyState {
        NONE,
        PRESSED,
        RELEASED,
        HOLDING,
    };

    KeyState get_key_state_(SDL_Scancode scancode) const {
        if(prev_key_states_[scancode] == 0) {
            if(curr_key_states_[scancode] == 0) {
                return KeyState::NONE;
            }
            else {
                return KeyState::PRESSED;
            }
        }
        else {
            if(curr_key_states_[scancode] == 0) {
                return KeyState::RELEASED;
            }
            else {
                return KeyState::HOLDING;
            }
        }
    }

    void process_inputs_();
    void update_game_();
    void render_();

    // game states
    float yaw_ = -90.0f, pitch_ = 0.0f;
    glm::vec3 camera_pos_;
    glm::vec3 camera_dir_;
    glm::vec3 camera_right_;
    glm::vec3 camera_up_;

public:
    Game() noexcept : window_(nullptr), ticks_(0llu), quit_(false), camera_pos_(0.0f, 0.0f, -5.0f) {}
    ~Game() noexcept {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    auto window() const noexcept { return window_; }
    auto delta_time() const noexcept { return delta_time_; }
    auto& camera_pos() const noexcept { return camera_pos_; }
    auto& camera_dir() const noexcept { return camera_dir_; }
    auto& camera_up() const noexcept { return camera_up_; }

    bool init(int window_width, int window_height);

    std::vector<const char*> enum_instance_extensions();

    template<typename F>
    void main_loop(F func) {
        using namespace std::chrono;

        while(!quit_) {
            process_inputs_();

            update_game_();

            // rendering process
            func();

            SDL_SetWindowTitle(window_, std::format("{:.2f} fps, camera pos: ({:.2f}, {:.2f}, {:.2f}), camera dir: ({:.2f}, {:.2f}, {:.2f})", 1.0f / delta_time_, camera_pos_.x, camera_pos_.y, camera_pos_.z, camera_dir_.x, camera_dir_.y, camera_dir_.z).c_str());
        }
    }
};