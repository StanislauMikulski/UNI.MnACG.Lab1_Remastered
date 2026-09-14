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
  double f_max = 2.0 * 5 * M_PI;
  double a = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / (2.5 * f_max);
  double df = 0.005;
  double alpha = M_PI / 12;

  int center_x = SCREEN_WIDTH / 2;
  int center_y = SCREEN_HEIGHT / 2;

  std::vector<std::pair<int, int>> points;
  for (double f = 0; f <= f_max; f += df) {
    double p = a * f;
    int x = center_x + static_cast<int>(p * cos(f));
    int y = center_y - static_cast<int>(p * sin(f));
    points.push_back({x, y});
  }

  auto draw_spiral = [&](double rot_angle) {
    SDL_FillRect(s, NULL, RGB32(0, 0, 0));
    int prev_x = center_x;
    int prev_y = center_y;
    bool first = true;
    for (const auto& pt : points) {
      double dx = pt.first - center_x;
      double dy = pt.second - center_y;
      int rx = center_x + static_cast<int>(dx * cos(rot_angle) + dy * sin(rot_angle));
      int ry = center_y - static_cast<int>(dx * sin(rot_angle) - dy * cos(rot_angle));

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
  };

  double rot_angle = 0.0;

  for (size_t i = 0; i < points.size(); ++i) {
    double dx = points[i].first - center_x;
    double dy = points[i].second - center_y;
    int rx = center_x + static_cast<int>(dx * cos(rot_angle) + dy * sin(rot_angle));
    int ry = center_y - static_cast<int>(dx * sin(rot_angle) - dy * cos(rot_angle));

    if (i > 0) {
      double pdx = points[i-1].first - center_x;
      double pdy = points[i-1].second - center_y;
      int prev_rx = center_x + static_cast<int>(pdx * cos(rot_angle) + pdy * sin(rot_angle));
      int prev_ry = center_y - static_cast<int>(pdx * sin(rot_angle) - pdy * cos(rot_angle));

      int steps = std::max(std::abs(rx - prev_rx), std::abs(ry - prev_ry));
      for (int st = 0; st <= steps; ++st) {
        int lerp_x = prev_rx + (rx - prev_rx) * st / (steps == 0 ? 1 : steps);
        int lerp_y = prev_ry + (ry - prev_ry) * st / (steps == 0 ? 1 : steps);
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

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) {
        return;
      }
    }
  }

  bool running = true;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) {
        running = false;
      } else if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
          case SDLK_a:
            rot_angle += alpha * 0.1;
            draw_spiral(rot_angle);
            break;
          case SDLK_b:
            rot_angle -= alpha * 0.1;
            draw_spiral(rot_angle);
            break;
          default:
            break;
        }
      }
    }
    SDL_Delay(16);
  }
}
