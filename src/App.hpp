#include <SDL2/SDL.h>

class App {
    SDL_Window* window_;
    SDL_Event event_;
    bool quit_;

public:
    App() noexcept : window_(nullptr), event_(), quit_(false) {}
    ~App() noexcept {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    bool init(int window_width, int window_height);

    template<typename F>
    void main_loop(F func) {
        while(!quit_) {
            func();
            while(SDL_PollEvent(&event_)) {
                if(event_.type == SDL_QUIT) quit_ = true;
                else if(event_.type == SDL_KEYDOWN && event_.key.keysym.sym == SDLK_ESCAPE) quit_ = true;
            }
        }
    }
};