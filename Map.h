#pragma once
#include <SDL.h>
#include <string>
#include <vector>

// Định nghĩa các hằng số cho map
const int TILE_SIZE = 32;
const int MAP_HEIGHT_TILES = 19; // 600 / 32 ~ 19
const int MAP_WIDTH_TILES = 25;  // 800 / 32 = 25

class Map {
public:
    Map(SDL_Renderer* renderer, const std::string& tileTexturePath);
    ~Map();

    void loadMap(const std::string& path);
    void render(SDL_Renderer* renderer);
    // Hàm mới: Lấy loại ô tại tọa độ pixel (x, y)
    int getTileTypeFromPixelCoords(int x, int y);

private:
    // Ma trận lưu dữ liệu của map
    std::vector<std::vector<int>> levelData;
    SDL_Texture* tileTexture;
};