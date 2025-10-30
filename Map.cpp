#include "Map.h"
#include "TextureManager.h"
#include <fstream>
#include <iostream>

Map::Map(SDL_Renderer* renderer, const std::string& tileTexturePath) {
    tileTexture = TextureManager::LoadTexture(tileTexturePath, renderer);
    // Khởi tạo ma trận với kích thước và giá trị mặc định là 0 (không khí)
    levelData.resize(MAP_HEIGHT_TILES, std::vector<int>(MAP_WIDTH_TILES, 0));
}

Map::~Map() {
    SDL_DestroyTexture(tileTexture);
}

void Map::loadMap(const std::string& path) {
    std::ifstream mapFile(path);
    if (!mapFile.is_open()) {
        std::cout << "Failed to open map file: " << path << std::endl;
        return;
    }

    for (int row = 0; row < MAP_HEIGHT_TILES; ++row) {
        for (int col = 0; col < MAP_WIDTH_TILES; ++col) {
            mapFile >> levelData[row][col];
        }
    }
    mapFile.close();
}

void Map::render(SDL_Renderer* renderer) {
    for (int row = 0; row < MAP_HEIGHT_TILES; ++row) {
        for (int col = 0; col < MAP_WIDTH_TILES; ++col) {
            if (levelData[row][col] == 1) { // Nếu là khối đất
                SDL_Rect destRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderCopy(renderer, tileTexture, NULL, &destRect);
            }
        }
    }
}

int Map::getTileTypeFromPixelCoords(int x, int y) {
    // Chuyển đổi tọa độ pixel sang tọa độ ô trong ma trận
    int tileCol = x / TILE_SIZE;
    int tileRow = y / TILE_SIZE;

    // Kiểm tra xem có nằm ngoài map không
    if (tileRow < 0 || tileRow >= MAP_HEIGHT_TILES || tileCol < 0 || tileCol >= MAP_WIDTH_TILES) {
        return 0; // Coi như là không khí
    }
    return levelData[tileRow][tileCol];
}