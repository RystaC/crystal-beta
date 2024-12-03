#include "Game.hpp"

bool Game::init(int window_width, int window_height) {
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

    SDL_SetRelativeMouseMode(SDL_TRUE);
    is_mouse_relative_ = true;

    curr_key_states_ = SDL_GetKeyboardState(nullptr);

    // initialize ticks
    using namespace std::chrono;
    ticks_ = duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();

    return true;
}

void Game::process_inputs_() {
    // record previous keyboard states
    std::copy(curr_key_states_, curr_key_states_ + SDL_NUM_SCANCODES, prev_key_states_.begin());

    while(SDL_PollEvent(&event_)) {
        if(event_.type == SDL_QUIT) quit_ = true;
    }

    // keyboard input
    if(get_key_state_(SDL_SCANCODE_ESCAPE) == KeyState::RELEASED) quit_ = true;
    if(curr_key_states_[SDL_SCANCODE_LALT]) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        is_mouse_relative_ = false;
    }
    else {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        is_mouse_relative_ = true;
    }

    constexpr float VELOCITY = 5.0f;

    if(curr_key_states_[SDL_SCANCODE_A]) {
        camera_pos_ -= VELOCITY * camera_right_ * delta_time_;
    }
    if(curr_key_states_[SDL_SCANCODE_D]) {
        camera_pos_ += VELOCITY * camera_right_ * delta_time_;
    }
    if(curr_key_states_[SDL_SCANCODE_S]) {
        camera_pos_ -= VELOCITY * camera_dir_ * delta_time_;
    }
    if(curr_key_states_[SDL_SCANCODE_W]) {
        camera_pos_ += VELOCITY * camera_dir_ * delta_time_;
    }
    if(get_key_state_(SDL_SCANCODE_LSHIFT) == KeyState::NONE) {
        if(curr_key_states_[SDL_SCANCODE_SPACE]) {
            camera_pos_ += VELOCITY * camera_up_ * delta_time_;
        }
    }
    else if(get_key_state_(SDL_SCANCODE_LSHIFT) == KeyState::HOLDING) {
        if(curr_key_states_[SDL_SCANCODE_SPACE]) {
            camera_pos_ -= VELOCITY * camera_up_ * delta_time_;
        }
    }

    // mouse input
    constexpr float SENSITIVITY = 0.1f;
    if(is_mouse_relative_) {
        mouse_buttons_ = SDL_GetRelativeMouseState(&mouse_x_, &mouse_y_);
        yaw_ += mouse_x_ * SENSITIVITY;
        pitch_ -= mouse_y_ * SENSITIVITY;
        if(pitch_ > 89.0f) {
            pitch_ = 89.0f;
        }
        if(pitch_ < -89.0f) {
            pitch_ = -89.0f;
        }
    }
    else {
        mouse_buttons_ = SDL_GetMouseState(&mouse_x_, &mouse_y_);
    }

    camera_dir_ = glm::normalize(
        glm::vec3(
            glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
            // for Vulkan coordinate system
            -glm::sin(glm::radians(pitch_)),
            // this camera assume direction points look at
            -glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_))
        )
    );

    auto look_up = glm::vec3(0.0f, -1.0f, 0.0f);
    camera_right_ = glm::normalize(glm::cross(camera_dir_, look_up));

    camera_up_ = glm::cross(camera_right_, camera_dir_);
}

void Game::update_game_() {
    // limit frame rate (ticks + [miliseconds])
    // while(!SDL_TICKS_PASSED(SDL_GetTicks64(), ticks_ + 16));

    // resolution: miliseconds
    // auto current_ticks = SDL_GetTicks64();
    // auto delta_ticks = current_ticks - ticks_;
    // delta_time_ = delta_ticks / 1000.0f;
    // ticks_ = current_ticks;

    // resolution: microseconds (nanoseconds can be used. but it is too fine)
    using namespace std::chrono;
    auto current_ticks = duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    auto delta_ticks = current_ticks - ticks_;
    // 1 second = 10^6 microseconds
    delta_time_ = delta_ticks / (1000.0f * 1000.0f);
    ticks_ = current_ticks;

    if(delta_time_ > 0.05f) {
        delta_time_ = 0.05f;
    }
}

void Game::render_() {

}

std::vector<const char*> Game::enum_instance_extensions() {
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