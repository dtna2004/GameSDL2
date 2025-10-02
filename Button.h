#pragma once
#include <SDL.h>
#include <string>

class Button {
public:
    Button(const std::string& texturePath, int x, int y, int w, int h, SDL_Renderer* renderer);
    ~Button();

    void render(SDL_Renderer* renderer);
    bool isClicked(SDL_Event* e);

private:
    SDL_Texture* texture;
    SDL_Rect rect;
};