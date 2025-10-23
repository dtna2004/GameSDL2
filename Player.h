#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include "Projectile.h"


class Player {
public:
    Player(const std::string& texturePath, int x, int y, SDL_Renderer* renderer);
    ~Player();

    void handleEvent(SDL_Event& e, std::vector<Projectile*>& projectiles, SDL_Renderer* renderer);
    void update();
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect();

    float getPower() const { return power; }
    float getAngle() const { return angle; }
    bool isFacingRight() const { return facingRight; }

    int health = 3;

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    float angle = 45.0f;
    float power = 0.0f;
    bool isCharging = false;
    bool facingRight = true;
    float y_vel = 0.0f;
    bool onGround = false;
};