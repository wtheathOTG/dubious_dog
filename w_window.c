#include "w_window.h"

SDL_Window *sdl_window = NULL;

void W_Init(const unsigned int window_w, const unsigned int window_h) {
    SDL_Init(SDL_INIT_EVERYTHING);
    sdl_window = SDL_CreateWindow(
        "Dubious Dog",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_w,
        window_h,
        SDL_WINDOW_SHOWN
    );
}
void W_Shutdown() {
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}
SDL_Window* W_Get() {
    return sdl_window;
}