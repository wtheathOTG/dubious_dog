#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_main.h>  // helps avoid WinMain linker errors on Windows

#define WINDOW_TITLE "dubious dog"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
} Game;

bool sdl_initialized(Game *game);
void game_cleanup(Game *game);

int main(int argc, char* argv[]) {
    Game game = {
        .window = NULL,
        .renderer = NULL,
    };
    if (sdl_initialized(&game)) {
        game_cleanup(&game);
        exit(1);
    }

    SDL_Delay(1000);

    game_cleanup(&game);
    return 0;
}

bool sdl_initialized(Game *game) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr,"Error initializing SDL: %s\n", SDL_GetError());
        return true;
    }

    game->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    if (game->window == NULL) {
        fprintf(stderr,"Error creating window: %s\n", SDL_GetError());
        return true;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    if (game->renderer == NULL) {
        fprintf(stderr,"Error creating renderer: %s\n", SDL_GetError());
        return true;
    }

    return false;
}

void game_cleanup(Game *game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}