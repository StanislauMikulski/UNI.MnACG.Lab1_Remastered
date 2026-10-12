#include "draw.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>
#include <cassert>

#define RGB32(r, g, b) static_cast<uint32_t>((((static_cast<uint32_t>(b) << 8) | g) << 8) | r)

void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  assert(NULL != surface);
  assert(x >= 0 && x < SCREEN_WIDTH);
  assert(y >= 0 && y < SCREEN_HEIGHT);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  pixels[(y * surface->w) + x] = pixel;
}

Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
{
  assert(NULL != surface);
  assert(x >= 0 && x < SCREEN_WIDTH);
  assert(y >= 0 && y < SCREEN_HEIGHT);

  Uint32 *pixels = (Uint32 *)surface->pixels;
  return pixels[(y * surface->w) + x];
}

void draw(SDL_Surface *s, SDL_Renderer *renderer, SDL_Texture *texture, double &current_f, double f_max, double df, double a, double rot_angle)
{
    int center_x = SCREEN_WIDTH / 2;
    int center_y = SCREEN_HEIGHT / 2;

    if (current_f <= f_max) {
        current_f += df;
        if (current_f > f_max) current_f = f_max;
    }

    SDL_FillRect(s, NULL, RGB32(0, 0, 0));

    int prev_x = -1;
    int prev_y = -1;
    bool first = true;

    for (double f = 0; f <= current_f; f += df) {
        double p = a * f;
        double dx = p * cos(f);
        double dy = p * sin(f);

        int rx = center_x + static_cast<int>(dx * cos(rot_angle) - dy * sin(rot_angle));
        int ry = center_y - static_cast<int>(dx * sin(rot_angle) + dy * cos(rot_angle));

        if (!first) {
            int steps = std::max(std::abs(rx - prev_x), std::abs(ry - prev_y));
            for (int st = 0; st <= steps; ++st) {
                int lerp_x = prev_x + (rx - prev_x) * st / (steps == 0 ? 1 : steps);
                int lerp_y = prev_y + (ry - prev_y) * st / (steps == 0 ? 1 : steps);
                if (lerp_x >= 0 && lerp_x < SCREEN_WIDTH && lerp_y >= 0 && lerp_y < SCREEN_HEIGHT) {
                    put_pixel32(s, lerp_x, lerp_y, RGB32(255, 255, 255));
                }
            }
        }
        prev_x = rx;
        prev_y = ry;
        first = false;
    }

    SDL_UpdateTexture(texture, NULL, s->pixels, s->pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
