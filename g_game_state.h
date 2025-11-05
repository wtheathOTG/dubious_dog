#ifndef DUBIOUS_DOG_G_GAME_STATE_H
#define DUBIOUS_DOG_G_GAME_STATE_H

#define SDL_MAIN_HANDLED

#include <stdbool.h>
#include "typedefs.h"

typedef struct _game_state {
    unsigned int screen_w;
    unsigned int screen_h;
    double target_fps;
    double target_frame_time;
    double delta_time;
    bool is_running;
    bool is_paused;
    bool is_fps_capped;
    bool state_show_map;
    bool is_debug_mode;
} game_state_t;

game_state_t G_Init(const unsigned int screenw, const unsigned int screenh, int target_fps);
void G_FrameStart();
void G_FrameEnd(game_state_t *state);

#endif //DUBIOUS_DOG_G_GAME_STATE_H