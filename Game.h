#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
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
    void startGame(GameState mode); // <<-- KHAI BÁO CÒN THIẾU NẰM Ở ĐÂY
    void switchTurn();
    void createTextTexture(const std::string& text);
    void resetGame();

    bool isRunning = false;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    GameState gameState = GameState::MAIN_MENU;
    GameState stateBeforePause;

    Mix_Music* backgroundMusic = nullptr;
    bool isMusicOn = true;

    TTF_Font* gameFont = nullptr;
    SDL_Texture* timerTexture = nullptr;
    SDL_Rect timerRect;

    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* pauseOverlay = nullptr;
    SDL_Texture* heartTexture = nullptr;
    SDL_Texture* turn1Texture = nullptr;
    SDL_Texture* turn2Texture = nullptr;
    SDL_Texture* win1Texture = nullptr;
    SDL_Texture* win2Texture = nullptr;
    Map* map = nullptr;

    Button* playAgainButton = nullptr;
    Button* player1Button = nullptr;
    Button* player2Button = nullptr;
    Button* settingsButton = nullptr;
    Button* exitButton = nullptr;
    Button* musicToggleButton = nullptr;
    Button* backButton = nullptr;

    bool turnShouldEnd = false;
    Player* player1 = nullptr;
    Player* player2 = nullptr;
    std::vector<Projectile*> projectiles;
    std::vector<Explosion*> explosions;
    int currentPlayerTurn = 1;
    Uint32 turnStartTime = 0;
    const int TURN_DURATION = 20000;
    Uint32 turnNotificationEndTime = 0;
    int winner = 0;
};