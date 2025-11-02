#include "Game.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>
#include <random> // Thư viện cần thiết cho việc tạo số ngẫu nhiên

#ifndef PI
#define PI 3.14159265
#endif

const float MAX_POWER = 500.0f;

Game::Game() {}
Game::~Game() {}

void Game::init(const char* title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    isRunning = true;
    gameFont = TTF_OpenFont("font/arial.ttf", 32);

    // Tạo đối tượng Map, nhưng chưa load một map cụ thể nào
    map = new Map(renderer, "image/tile.png");

    // ĐIỀN DỮ LIỆU VÀO DANH SÁCH CÁC MAP CÓ SẴN
    availableMaps.push_back({
        "level1.map",           // Đường dẫn file map
        {2, 9},                // Vị trí xuất phát của P1 (cột 2, hàng 11)
        {21, 9}                // Vị trí xuất phát của P2 (cột 21, hàng 11)
    });
    availableMaps.push_back({
        "level2.map",
        {2, 8},
        {21, 8}
    });
    availableMaps.push_back({
        "level3.map",
        {1, 4},
        {22, 4}
    });

    // Tải các tài nguyên khác
    backgroundMusic = Mix_LoadMUS("sound/music.mp3");
    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1);
        isMusicOn = true;
    }

    backgroundTexture = TextureManager::LoadTexture("image/background.png", renderer);
    pauseOverlay = TextureManager::LoadTexture("image/pause.png", renderer);
    heartTexture = TextureManager::LoadTexture("image/heart.png", renderer);
    turn1Texture = TextureManager::LoadTexture("image/turn_player1.png", renderer);
    turn2Texture = TextureManager::LoadTexture("image/turn_player2.png", renderer);
    win1Texture = TextureManager::LoadTexture("image/win1.png", renderer);
    win2Texture = TextureManager::LoadTexture("image/win2.png", renderer);

    playAgainButton = new Button("image/playagain.png", (800 - 250) / 2, 400, 250, 80, renderer);
    player1Button = new Button("image/1_player_button.png", 100, 350, 250, 80, renderer);
    player2Button = new Button("image/2_player_button.png", 450, 350, 250, 80, renderer);
    settingsButton = new Button("image/settings_button.png", 100, 450, 250, 80, renderer);
    exitButton = new Button("image/exit_button.png", 450, 450, 250, 80, renderer);
    musicToggleButton = new Button("image/volumn_button.png", 275, 200, 250, 80, renderer);
    backButton = new Button("image/exit_button.png", 275, 300, 250, 80, renderer);

    gameState = GameState::MAIN_MENU;
}

void Game::startGame(GameState mode) {
    gameState = mode;

    // 1. CHỌN MAP NGẪU NHIÊN
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, availableMaps.size() - 1);

    int randomMapIndex = distrib(gen);
    MapData selectedMap = availableMaps[randomMapIndex];

    // 2. TẢI MAP ĐÃ CHỌN
    map->loadMap(selectedMap.mapFilePath);
    std::cout << "Loading map: " << selectedMap.mapFilePath << std::endl;

    // 3. TẠO NGƯỜI CHƠI TẠI VỊ TRÍ TƯƠNG ỨNG
    player1 = new Player("image/male.png", selectedMap.player1StartPos.x * TILE_SIZE, selectedMap.player1StartPos.y * TILE_SIZE, renderer);
    player2 = new Player("image/female.png", selectedMap.player2StartPos.x * TILE_SIZE, selectedMap.player2StartPos.y * TILE_SIZE, renderer);

    switchTurn();
}

void Game::resetGame() {
    if(player1) { delete player1; player1 = nullptr; }
    if(player2) { delete player2; player2 = nullptr; }

    for (auto p : projectiles) delete p;
    projectiles.clear();
    for (auto e : explosions) delete e;
    explosions.clear();

    winner = 0;
    currentPlayerTurn = 1;
    gameState = GameState::MAIN_MENU;
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT) isRunning = false;

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p && !event.key.repeat) {
        if (gameState == GameState::PLAYING_1P || gameState == GameState::PLAYING_2P) {
            stateBeforePause = gameState;
            gameState = GameState::PAUSED;
            if(isMusicOn) Mix_PauseMusic();
        } else if (gameState == GameState::PAUSED) {
            gameState = stateBeforePause;
            if(isMusicOn) Mix_ResumeMusic();
        }
    }

    switch (gameState) {
        case GameState::MAIN_MENU:
            if (player1Button->isClicked(&event)) startGame(GameState::PLAYING_1P);
            if (player2Button->isClicked(&event)) startGame(GameState::PLAYING_2P);
            if (settingsButton->isClicked(&event)) gameState = GameState::SETTINGS;
            if (exitButton->isClicked(&event)) isRunning = false;
            break;
        case GameState::SETTINGS:
            if (musicToggleButton->isClicked(&event)) {
                isMusicOn = !isMusicOn;
                if(isMusicOn) Mix_ResumeMusic(); else Mix_PauseMusic();
            }
            if (backButton->isClicked(&event)) gameState = GameState::MAIN_MENU;
            break;
        case GameState::GAME_OVER:
            if (playAgainButton->isClicked(&event)) resetGame();
            break;
        case GameState::PLAYING_1P:
        case GameState::PLAYING_2P:
            if (currentPlayerTurn == 1) player1->handleEvent(event, projectiles, renderer);
            else player2->handleEvent(event, projectiles, renderer);
            break;
        default: break;
    }
}

void Game::switchTurn() {
    currentPlayerTurn = (currentPlayerTurn == 1) ? 2 : 1;
    turnStartTime = SDL_GetTicks();
    turnNotificationEndTime = SDL_GetTicks() + 2000;
}


void Game::update() {
    if (gameState != GameState::PLAYING_1P && gameState != GameState::PLAYING_2P) return;

    turnShouldEnd = false;

    player1->update(map);
    player2->update(map);

    if (player1->isOutOfBound(600)) {
        gameState = GameState::GAME_OVER; winner = 2; return;
    }
    if (player2->isOutOfBound(600)) {
        gameState = GameState::GAME_OVER; winner = 1; return;
    }

    Uint32 currentTime = SDL_GetTicks();
    int remainingTime = (TURN_DURATION - (currentTime - turnStartTime)) / 1000;
    if (remainingTime < 0) remainingTime = 0;
    createTextTexture(std::to_string(remainingTime));

    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i]->update();
        SDL_Rect projRect = projectiles[i]->getRect();
        int projCenterX = projRect.x + projRect.w / 2;
        int projCenterY = projRect.y + projRect.h / 2;

        bool projectileDestroyed = false;

        if (map->getTileTypeFromPixelCoords(projCenterX, projCenterY) != 0) {
            projectileDestroyed = true;
            turnShouldEnd = true;
        }

        Player* targetPlayer = (currentPlayerTurn == 1) ? player2 : player1;
        SDL_Rect targetHitbox = { targetPlayer->getRect().x + 15, targetPlayer->getRect().y + 10, 34, 54 };
        if (SDL_HasIntersection(&projRect, &targetHitbox)) {
            targetPlayer->health--;
            projectileDestroyed = true;
            turnShouldEnd = true;

            if (targetPlayer->health <= 0) {
                gameState = GameState::GAME_OVER;
                winner = currentPlayerTurn;
                return;
            }
        }

        if (projectileDestroyed) {
            explosions.push_back(new Explosion(renderer, projRect.x, projRect.y));
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            i--;
        }
    }

    for (int i = 0; i < explosions.size(); ++i) {
        explosions[i]->update();
        if (explosions[i]->isFinished()) {
            delete explosions[i];
            explosions.erase(explosions.begin() + i);
            i--;
        }
    }

    if (projectiles.empty() && currentTime > turnStartTime + TURN_DURATION) {
        turnShouldEnd = true;
    }

    if (turnShouldEnd) {
        switchTurn();
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }

    if (gameState == GameState::MAIN_MENU || gameState == GameState::SETTINGS) {
        if(gameState == GameState::MAIN_MENU) {
            player1Button->render(renderer); player2Button->render(renderer);
            settingsButton->render(renderer); exitButton->render(renderer);
        } else {
            musicToggleButton->render(renderer); backButton->render(renderer);
        }
    }
    else if (gameState == GameState::GAME_OVER) {
        if (map) map->render(renderer);
        if (player1) player1->render(renderer);
        if (player2) player2->render(renderer);

        SDL_Texture* winTexture = (winner == 1) ? win1Texture : win2Texture;
        if (winTexture) {
            SDL_Rect winRect = {(800 - 400) / 2, (600 - 200) / 2, 400, 200};
            SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
        }

        playAgainButton->render(renderer);
    }
    else { // PLAYING hoặc PAUSED
        if (map) map->render(renderer);
        if (player1) player1->render(renderer);
        if (player2) player2->render(renderer);

        for (auto& p : projectiles) p->render(renderer);
        for (auto& e : explosions) e->render(renderer);

        if (player1 && heartTexture) { for (int i = 0; i < player1->health; ++i) { SDL_Rect r = {10 + (i * 35), 10, 32, 32}; SDL_RenderCopy(renderer, heartTexture, NULL, &r); } }
        if (player2 && heartTexture) { for (int i = 0; i < player2->health; ++i) { SDL_Rect r = {790 - 32 - (i * 35), 10, 32, 32}; SDL_RenderCopy(renderer, heartTexture, NULL, &r); } }
        if (timerTexture) { SDL_RenderCopy(renderer, timerTexture, NULL, &timerRect); }
        if (SDL_GetTicks() < turnNotificationEndTime) { SDL_Rect r = {(800 - 300) / 2, (600 - 100) / 2, 300, 100}; SDL_RenderCopy(renderer, (currentPlayerTurn == 1) ? turn1Texture : turn2Texture, NULL, &r); }

        Player* currentPlayer = (currentPlayerTurn == 1) ? player1 : player2;
        if (currentPlayer) {
            SDL_Rect powerBarBg = { currentPlayer->getRect().x, currentPlayer->getRect().y - 20, 100, 10 };
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &powerBarBg);
            float powerPercent = currentPlayer->getPower() / MAX_POWER;
            SDL_Rect powerBarFg = { powerBarBg.x, powerBarBg.y, (int)(100 * powerPercent), 10 };
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
            SDL_RenderFillRect(renderer, &powerBarFg);

            float angle = currentPlayer->getAngle();
            bool facingRight = currentPlayer->isFacingRight();
            float shootingAngle = facingRight ? angle : 180.0f - angle;
            float angleRad = shootingAngle * PI / 180.0f;
            int startX = currentPlayer->getRect().x + currentPlayer->getRect().w / 2;
            int startY = currentPlayer->getRect().y + currentPlayer->getRect().h / 2;
            int endX = startX + (int)(cos(angleRad) * 40);
            int endY = startY - (int)(sin(angleRad) * 40);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
        }

        if (gameState == GameState::PAUSED && pauseOverlay) {
             SDL_RenderCopy(renderer, pauseOverlay, NULL, NULL);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(pauseOverlay);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(turn1Texture);
    SDL_DestroyTexture(turn2Texture);
    SDL_DestroyTexture(win1Texture);
    SDL_DestroyTexture(win2Texture);
    SDL_DestroyTexture(timerTexture);
    delete player1Button;
    delete player2Button;
    delete settingsButton;
    delete exitButton;
    delete musicToggleButton;
    delete backButton;
    delete playAgainButton;
    delete player1;
    delete player2;
    delete map;
    for (auto p : projectiles) delete p;
    projectiles.clear();
    for (auto e : explosions) delete e;
    explosions.clear();
    TTF_CloseFont(gameFont);
    Mix_FreeMusic(backgroundMusic);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::createTextTexture(const std::string& text) {
    if (timerTexture) SDL_DestroyTexture(timerTexture);
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(gameFont, text.c_str(), textColor);
    if (textSurface) {
        timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        timerRect = { (800 - textSurface->w) / 2, 10, textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
    }
}