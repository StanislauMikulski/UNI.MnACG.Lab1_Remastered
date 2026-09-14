#pragma once
#include <SDL2/SDL.h>
#include <cassert>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void draw(SDL_Surface *s, SDL_Renderer *renderer, SDL_Texture *texture);
