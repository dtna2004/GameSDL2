#pragma once
#include <SDL.h>
#include <string>
#include "TextureManager.h"

class Projectile {
public:
    Projectile(SDL_Renderer* renderer, float x, float y, float angle, float power);
    ~Projectile();

    void update();
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect();

private:
    SDL_Texture* texture;
    float x_pos, y_pos;
    float vx, vy;
    const float GRAVITY = 98.0f; // Adjusted gravity for pixel scale
};