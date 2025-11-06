#include "r_renderer.h"
#include <SDL.h>
#include <stdbool.h>

SDL_Window* window;
SDL_Renderer* sdl_renderer;
SDL_Texture* screen_texture;
unsigned int screenw, screenh;

bool is_debug_mode = false;
unsigned int *screen_buffer = NULL;
int screen_buffer_size = 0;

sectors_queue_t sectors_queue;

void R_ShutdownScreen() {
    if (screen_texture) {
        SDL_DestroyTexture(screen_texture);
    }
    if (screen_buffer != NULL) free(screen_buffer);
}

void R_Shutdown() {
    R_ShutdownScreen();
    SDL_DestroyRenderer(sdl_renderer);
}

void R_UpdateScreen() {
    SDL_UpdateTexture(screen_texture, NULL, screen_buffer, screenw * sizeof(unsigned int));
    SDL_RenderCopy(sdl_renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);
}

void R_InitScreen(int w, int h) {
    screen_buffer_size = sizeof(unsigned int) * w * h;
    screen_buffer = (unsigned int*)malloc(screen_buffer_size);
    if (screen_buffer == NULL) {
        screen_buffer_size = -1;
        printf("Error initializing screen buffer!\n");
        R_Shutdown();
    }

    memset(screen_buffer, 0, screen_buffer_size);

    screen_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        w,
        h
    );

    if (screen_texture == NULL) {
        printf("Error creating screen texture!\n");
        R_Shutdown();
    }
}

void R_Init(SDL_Window* main_win, game_state_t *game_state) {
    window = main_win;
    screenw = game_state->screen_w / 2;
    screenh = game_state->screen_h / 2;

    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    R_InitScreen(screenw, screenh);
    SDL_RenderSetLogicalSize(sdl_renderer, screenw, screenh);
}

void R_DrawPoint(int x, int y, unsigned int color) {
    bool is_out_of_bounds = (x < 0 || x > screenw || y < 0 || y >= screenh);
    bool is_outside_mem_buff = (screenw * y + x) >= (screenw * screenh);

    if (is_out_of_bounds || is_outside_mem_buff) return;

    screen_buffer[screenw * y + x] = color;
}

void R_DrawLine(int x0, int y0, int x1, int y1, unsigned int color) {
    //Bresenham's Line Drawing Algorithm
    int dx;
    if (x1 > x0)
        dx = x1 - x0;
    else
        dx = x0 - x1;

    int dy;
    if (y1 > y0)
        dy = y1 - y0;
    else
        dy = y0 - y1;

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        R_DrawPoint(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;

        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }

    if (is_debug_mode)
    {
        R_UpdateScreen();
        SDL_Delay(10);
    }
}

void R_ClearScreenBuffer() {
    memset(screen_buffer, 0, sizeof(uint32_t) * screenw * screenh);
}

void R_ClipBehindPlayer(double *ax, double *ay, double bx, double by) {
    double px1 = 1;
    double py1 = 1;
    double px2 = 200;
    double py2 = 1;

    double a = (px1 - px2) * (*ay - py2) - (py1 - py2) * (*ax - px2);
    double b = (py1 - py2) * (*ax - bx) - (px1 - px2) * (*ay - by);

    double t = a / b;

    *ax = *ax - (t * (bx - *ax));
    *ay = *ay - (t * (by - *ay));
}

void R_RenderSectors(player_t *player, game_state_t *game_state) {
    double screen_half_w = screenw / 2;
    double screen_half_h = screenh / 2;
    double fov = 300;
    unsigned int wall_color = 0xFFFF00FF;

    R_ClearScreenBuffer();

    for (int i = 0; i < sectors_queue.num_sectors; i++) {
        sector_t *s = &sectors_queue.sectors[i];
        int sector_h = s->height;
        int sector_e = s->elevation;
        unsigned int sector_clr = s->color;

        for (int k = 0; k < s->num_walls; k++) {
            wall_t *w = &s->walls[k];

            //displace world based on player's position
            double dx1 = w->a.x - player->position.x;
            double dy1 = w->a.y - player->position.y;
            double dx2 = w->b.x - player->position.x;
            double dy2 = w->b.y - player->position.y;

            //rotate the world around the player
            double SN = sin(player->dir_angle);
            double CN = cos(player->dir_angle);
            double wx1 = dx1 * SN - dy1 * CN;
            double wz1 = dx1 * CN + dy1 * SN;
            double wx2 = dx2 * SN - dy2 * CN;
            double wz2 = dx2 * CN + dy2 * SN;

            //if z1 and z2 < 0 (wall completely behind player) -- skip it
            //if z1 or z2 is behind the player -- clip it
            if (wz1 < 0 && wz2 < 0) continue;
            if (wz1 < 0) R_ClipBehindPlayer(&wx1, &wz1, wx2, wz2);
            else if (wz2 < 0) R_ClipBehindPlayer(&wx2, &wz2, wx1, wz1);

            //calc wall height based on distance
            double wh1 = (sector_h / wz1) * fov;
            double wh2 = (sector_h / wz2) * fov;

            //convert to screen space
            double sx1 = (wx1 / wz1) * fov;
            double sy1 = ((game_state->screen_h + player->z) / wz1);
            double sx2 = (wx2 / wz2) * fov;
            double sy2 = ((game_state->screen_h + player->z) / wz2);

            //calc wall elevation from floor
            double s_level1 = (sector_e / wz1) * fov;
            double s_level2 = (sector_e / wz2) * fov;
            sy1 -= s_level1;
            sy2 -= s_level2;

            //construct portal top and bottom
            double pbh1 = 0;
            double pbh2 = 0;
            double pth1 = 0;
            double pth2 = 0;
            if (w->is_portal) {
                pth1 = (w->portal_top_height / wz1) * fov;
                pth2 = (w->portal_top_height / wz2) * fov;
                pbh1 = (w->portal_bot_height / wz1) * fov;
                pbh2 = (w->portal_bot_height / wz2) * fov;
            }

            //set screen-space origin to center of the screen
            sx1 += screen_half_w;
            sy1 += screen_half_h;
            sx2 += screen_half_w;
            sy2 += screen_half_h;

            //top
            R_DrawLine(sx1, sy1 - wh1, sx2, sy2 - wh2, wall_color);
            //bottom
            R_DrawLine(sx1, sy1, sx2, sy2, wall_color);
            //left edge
            R_DrawLine(sx1, sy1 - wh1, sx1, sy1, wall_color);
            //right edge
            R_DrawLine(sx2, sy2 - wh2, sx2, sy2, wall_color);
        }
    }
}

void R_Render(player_t *player, game_state_t *game_state) {
    is_debug_mode = game_state->is_debug_mode;
    R_RenderSectors(player, game_state);
    R_UpdateScreen();
}
void R_DrawWalls(player_t *player, game_state_t *game_state) {

}

sector_t R_CreateSector(int height, int elevation, unsigned int color, unsigned int ceil_clr, unsigned int floor_clr) {
    static int sector_id = 0;
    sector_t sector = {0};
    sector.num_walls = 0;
    sector.height = height;
    sector.elevation = elevation;
    sector.color = color;
    sector.ceil_clr = ceil_clr;
    sector.floor_clr = floor_clr;
    sector.id = ++sector_id;

    return sector;
}

void R_SectorAddWall(sector_t *sector, wall_t vertices) {
    sector->walls[sector->num_walls] = vertices;
    sector->num_walls++;
}

void R_AddSectorToQueue(sector_t *sector) {
    sectors_queue.sectors[sectors_queue.num_sectors] = *sector;
    sectors_queue.num_sectors++;
}

wall_t R_CreateWall(int ax, int ay, int bx, int by) {
    wall_t w;
    w.a.x = ax;
    w.a.y = ay;
    w.b.x = bx;
    w.b.y = by;
    w.is_portal = false;

    return w;
}

wall_t R_CreatePortal(int ax, int ay, int bx, int by, int th, int bh) {

}