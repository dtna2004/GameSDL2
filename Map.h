#pragma once
#include <SDL.h>
#include <string>
#include "TextureManager.h"

class Map {
public:
    Map(SDL_Renderer* renderer, const std::string& imagePath);
    ~Map();

    void render(SDL_Renderer* renderer);
    bool isSolidAt(int x, int y); // Kiểm tra xem pixel tại (x, y) có phải là đất không

private:
    SDL_Texture* texture;
    SDL_Surface* surface; // Giữ lại surface để kiểm tra pixel
};