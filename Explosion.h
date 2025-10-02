#pragma once
#include <SDL.h>
#include <string>
#include "TextureManager.h"

class Explosion {
public:
    Explosion(SDL_Renderer* renderer, int x, int y);
    ~Explosion();

    void update();
    void render(SDL_Renderer* renderer);
    bool isFinished();

private:
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;

    int frameCount = 4;
    int currentFrame = 0;
    int frameWidth, frameHeight;
    Uint32 lastFrameTime = 0;
    int frameDuration = 100; // 100ms per frame
};