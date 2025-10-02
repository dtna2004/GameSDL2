//
// Created by ASUS on 02/10/2025.
//#include "Projectile.h"
#include <cmath>
#include "Projectile.h"
#define PI 3.14159265

Projectile::Projectile(SDL_Renderer* renderer, float x, float y, float angle, float power) {
    texture = TextureManager::LoadTexture("image/projectile.png", renderer);
    x_pos = x;
    y_pos = y;

    // Convert angle to radians for math functions
    float angleRad = angle * PI / 180.0f;

    // Calculate initial velocities
    vx = power * cos(angleRad);
    vy = -power * sin(angleRad); // Negative because SDL's Y-axis is inverted
}

Projectile::~Projectile() {
    SDL_DestroyTexture(texture);
}

void Projectile::update() {
    // Apply gravity
    vy += GRAVITY * (1.0f / 60.0f); // Assuming 60 FPS

    // Update position
    x_pos += vx * (1.0f / 60.0f);
    y_pos += vy * (1.0f / 60.0f);
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_Rect destRect = { (int)x_pos, (int)y_pos, 32, 32 };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
}

SDL_Rect Projectile::getRect() {
    return { (int)x_pos, (int)y_pos, 32, 32 };
}