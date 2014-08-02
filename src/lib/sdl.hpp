#ifndef SDL_HPP_INCLUDED
#define SDL_HPP_INCLUDED

#include "SDL2/SDL.h"

struct NativeWindow {
    SDL_Window * window;
    SDL_GLContext context;
};

#endif
