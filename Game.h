#pragma once
#include <SDL.h>
#include <SDL_mixer.h> // Thêm thư viện mixer
#include <vector>
#include "Player.h"
#include "Button.h"
#include "Projectile.h"
#include "Explosion.h"
#include "Map.h" // Thêm Map header

enum class GameState {
    MAIN_MENU,
    PLAYING_1P,
    PLAYING_2P, // Thêm trạng thái 2 người chơi
    PAUSED,     // Thêm trạng thái tạm dừng
    SETTINGS,   // Thêm trạng thái cài đặt
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() { return isRunning; }

private:
    void switchTurn();

    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    GameState gameState = GameState::MAIN_MENU;
    GameState stateBeforePause; // Lưu trạng thái trước khi pause

    // Audio
    Mix_Music* backgroundMusic = nullptr;

    // Textures
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* pauseOverlay = nullptr; // Texture cho màn hình pause
    Map* map = nullptr; // Thay thế mapTexture bằng đối tượng Map

    // Menu Objects
    Button* player1Button = nullptr;
    Button* player2Button = nullptr; // Thêm nút 2 người chơi
    Button* settingsButton = nullptr; // Thêm nút cài đặt
    Button* exitButton = nullptr;

    // Game Objects
    Player* player1 = nullptr;
    Player* player2 = nullptr;
    std::vector<Projectile*> projectiles;
    std::vector<Explosion*> explosions;
    int currentPlayerTurn = 1;
    Uint32 turnStartTime = 0;
    const int TURN_DURATION = 20000; // 20 seconds
};