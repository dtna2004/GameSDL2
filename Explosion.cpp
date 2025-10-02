#include "Explosion.h"

Explosion::Explosion(SDL_Renderer* renderer, int x, int y) {
    texture = TextureManager::LoadTexture("image/explosion.png", renderer);
    SDL_QueryTexture(texture, NULL, NULL, &frameWidth, &frameHeight);
    frameWidth /= frameCount; // Divide total width by number of frames

    srcRect = { 0, 0, frameWidth, frameHeight };
    destRect = { x - frameWidth / 2, y - frameHeight / 2, frameWidth, frameHeight }; // Center the explosion
    lastFrameTime = SDL_GetTicks();
}

Explosion::~Explosion() {
    SDL_DestroyTexture(texture);
}

void Explosion::update() {
    if (SDL_GetTicks() > lastFrameTime + frameDuration) {
        currentFrame++;
        srcRect.x = currentFrame * frameWidth;
        lastFrameTime = SDL_GetTicks();
    }
}

void Explosion::render(SDL_Renderer* renderer) {
    if (!isFinished()) {
        TextureManager::Draw(renderer, texture, srcRect, destRect);
    }
}

bool Explosion::isFinished() {
    return currentFrame >= frameCount;
}