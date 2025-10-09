#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const std::string& fileName, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(fileName.c_str());
    if (tempSurface == nullptr) {
        std::cout << "Unable to load image " << fileName << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // === THÊM MỚI: XÓA NỀN TRẮNG ===
    // Thiết lập màu trắng (R=255, G=255, B=255) làm màu trong suốt
    SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, 255, 255, 255));
    // ===================================

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (texture == nullptr) {
        std::cout << "Unable to create texture from " << fileName << "! SDL Error: " << SDL_GetError() << std::endl;
    }

    return texture;
}

void TextureManager::Draw(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect src, SDL_Rect dest) {
    SDL_RenderCopy(renderer, texture, &src, &dest);
}