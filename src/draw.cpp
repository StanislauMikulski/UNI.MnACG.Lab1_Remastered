#include "draw.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>

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

void draw(SDL_Surface *s, SDL_Renderer *renderer, SDL_Texture *texture)
{
  SDL_FillRect(s, NULL, RGB32(0, 0, 0));
  SDL_UpdateTexture(texture, NULL, s->pixels, s->pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  double f_max = 2.0 * 5 * M_PI;
  double a = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / (2.5 * f_max);
  double df = 0.005;
  double alpha = M_PI * 10;

  int center_x = SCREEN_WIDTH / 2;
  int center_y = SCREEN_HEIGHT / 2;

  int prev_x = center_x;
  int prev_y = center_y;
  bool first = true;
  static double rot_angle = 0.0;

  for (double f = 0; f <= f_max; f += df) {
    SDL_Event ev;


    double p = a * f;
    int x = center_x + static_cast<int>(p * cos(f));
    int y = center_y - static_cast<int>(p * sin(f));

    double dx = x - center_x;
    double dy = y - center_y;
    x = center_x + static_cast<int>(dx * cos(rot_angle) + dy * sin(rot_angle));
    y = center_y - static_cast<int>(dx * sin(rot_angle) - dy * cos(rot_angle));

    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
      if (!first) {
        int steps = std::max(std::abs(x - prev_x), std::abs(y - prev_y));
        for (int st = 0; st <= steps; ++st) {
          int lerp_x = prev_x + (x - prev_x) * st / (steps == 0 ? 1 : steps);
          int lerp_y = prev_y + (y - prev_y) * st / (steps == 0 ? 1 : steps);
          if (lerp_x >= 0 && lerp_x < SCREEN_WIDTH && lerp_y >= 0 && lerp_y < SCREEN_HEIGHT) {
            put_pixel32(s, lerp_x, lerp_y, RGB32(255, 255, 255));

            SDL_UpdateTexture(texture, NULL, s->pixels, s->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_Delay(1);
          }
        }
      }
      prev_x = x;
      prev_y = y;
      first = false;

          while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) {
        return;
      } else if (ev.type == SDL_KEYDOWN){
        switch (ev.key.keysym.sym){
          case SDLK_a:
            rot_angle += alpha * 0.1;
            break;
          case SDLK_b:
          continue;

        default:
          break;
        }
      }
    }
  }
  }
}
