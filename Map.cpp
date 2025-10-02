#include "Map.h"

Map::Map(SDL_Renderer* renderer, const std::string& imagePath) {
    surface = IMG_Load(imagePath.c_str());
    if (surface) {
        // Set color key để loại bỏ màu nền (nếu map của bạn có nền, ví dụ màu trắng)
        // SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));
        texture = SDL_CreateTextureFromSurface(renderer, surface);
    }
}

Map::~Map() {
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Map::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }
}

bool Map::isSolidAt(int x, int y) {
    // Nếu tọa độ nằm ngoài map, coi như không phải đất
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return false;
    }

    // Lấy con trỏ đến dữ liệu pixel
    Uint32* pixels = (Uint32*)surface->pixels;
    // Lấy pixel tại vị trí (x, y)
    Uint32 pixel = pixels[y * surface->w + x];

    // Lấy thành phần alpha (độ trong suốt) của pixel
    SDL_Color color;
    SDL_GetRGBA(pixel, surface->format, &color.r, &color.g, &color.b, &color.a);

    // Nếu alpha > 0, tức là pixel này không hoàn toàn trong suốt -> coi là đất
    return color.a > 0;
}