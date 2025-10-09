#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h> // <<-- THÊM THƯ VIỆN FONT
#include <vector>
#include <string> // <<-- THÊM
#include "Player.h"
#include "Button.h"
#include "Projectile.h"
#include "Explosion.h"
#include "Map.h"

enum class GameState {
    MAIN_MENU,
    PLAYING_1P,
    PLAYING_2P,
    PAUSED,
    SETTINGS,
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
    void createTextTexture(const std::string& text); // <<-- HÀM MỚI để tạo texture chữ

    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    GameState gameState = GameState::MAIN_MENU;
    GameState stateBeforePause;

    // Audio
    Mix_Music* backgroundMusic = nullptr;
    bool isMusicOn = true;

    // Font và Text
    TTF_Font* gameFont = nullptr; // <<-- FONT
    SDL_Texture* timerTexture = nullptr; // <<-- TEXTURE CHO ĐỒNG HỒ
    SDL_Rect timerRect;

    // Textures
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* pauseOverlay = nullptr;
    SDL_Texture* heartTexture = nullptr; // <<-- TEXTURE TRÁI TIM
    SDL_Texture* turn1Texture = nullptr; // <<-- TEXTURE THÔNG BÁO LƯỢT 1
    SDL_Texture* turn2Texture = nullptr; // <<-- TEXTURE THÔNG BÁO LƯỢT 2
    Map* map = nullptr;

    // Menu Objects
    Button* player1Button = nullptr;
    Button* player2Button = nullptr;
    Button* settingsButton = nullptr;
    Button* exitButton = nullptr;
    Button* musicToggleButton = nullptr;
    Button* backButton = nullptr;

    // Game Objects
    Player* player1 = nullptr;
    Player* player2 = nullptr;
    std::vector<Projectile*> projectiles;
    std::vector<Explosion*> explosions;
    int currentPlayerTurn = 1;
    Uint32 turnStartTime = 0;
    const int TURN_DURATION = 20000;

    // Logic cho thông báo lượt chơi
    Uint32 turnNotificationEndTime = 0; // <<-- BIẾN HẸN GIỜ
};