#include "g_game_state.h"

#include <SDL.h>

unsigned int frame_start = 0;

game_state_t G_Init(const unsigned int screenw, const unsigned int screenh, int target_fps) {
    game_state_t game_state;
    game_state.screen_w = screenw;
    game_state.screen_h = screenh;
    game_state.target_fps = target_fps;
    game_state.target_frame_time = 1.0 / game_state.target_fps;
    game_state.delta_time = game_state.target_frame_time;
    game_state.is_running = true;
    game_state.is_paused = false;
    game_state.is_fps_capped = false;
    game_state.state_show_map = false;
    game_state.is_debug_mode = false;

    return game_state;
}

void G_FrameStart() {
    frame_start = SDL_GetTicks();
}

void G_FrameEnd(game_state_t *state) {
    state->delta_time = (SDL_GetTicks() - frame_start) / 1000.0;

    if (state->delta_time < state->target_frame_time) {
        SDL_Delay((state->target_frame_time - state->delta_time) * 1000);
        state->delta_time = state->target_frame_time;
    }
}