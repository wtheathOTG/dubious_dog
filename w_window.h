#ifndef DUBIOUS_DOG_W_WINDOW_H
#define DUBIOUS_DOG_W_WINDOW_H

#include <SDL.h>

void W_Init(const unsigned int window_w, const unsigned int window_h);
void W_Shutdown();
SDL_Window* W_Get();

#endif //DUBIOUS_DOG_W_WINDOW_H