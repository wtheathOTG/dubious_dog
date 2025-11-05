#include <stdio.h>
#include <math.h>
#include "p_player.h"
#include "g_game_state.h"
#include "w_window.h"
#include "r_renderer.h"
#include "k_keyboard.h"

#define SCREENW 1024
#define SCREENH 768
#define FPS 120

void GameLoop(game_state_t *game_state, player_t *player) {
    while (game_state->is_running) {
        G_FrameStart();

        K_HandleEvents(game_state, player);
        R_Render(player, game_state);

        G_FrameEnd(game_state);
    }
}

int main() {
    game_state_t game_state = G_Init(SCREENW, SCREENH, FPS);
    player_t player = P_Init(40, 40, SCREENH * 10, M_PI / 2);
    K_InitKeymap();
    W_Init(SCREENW, SCREENH);
    R_Init(W_Get(), &game_state);


    sector_t s1 = R_CreateSector(10, 0, 0xd6382d, 0xf54236, 0x9c2921);
    int s1v[4*4] = {
        70, 220, 100, 220,
        100, 220, 100, 240,
        100, 240, 70, 240,
        70, 240, 70, 220
    };

    for (int i = 0; i < 16; i += 4) {
        wall_t w = R_CreateWall(s1v[i], s1v[i+1], s1v[i+2], s1v[i+3]);
        R_SectorAddWall(&s1, w);
    }

    R_AddSectorToQueue(&s1);

    GameLoop(&game_state, &player);

    return 0;
}