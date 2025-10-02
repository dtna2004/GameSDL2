#include "Button.h"
#include "TextureManager.h"

Button::Button(const std::string& texturePath, int x, int y, int w, int h, SDL_Renderer* renderer) {
    texture = TextureManager::LoadTexture(texturePath, renderer);
    rect = { x, y, w, h };
}

Button::~Button() {
    SDL_DestroyTexture(texture);
}

void Button::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

bool Button::isClicked(SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h) {
            return true;
            }
    }
    return false;
}