#include "Game.h"
#include "TextureManager.h"
#include <iostream>
#include <cmath>

#ifndef PI
#define PI 3.14159265
#endif

const int GROUND_LEVEL_FOR_PROJECTILE = 480;
const float MAX_POWER = 500.0f;

Game::Game() {}
Game::~Game() {}

void Game::init(const char* title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init(); // <<-- KHỞI TẠO HỆ THỐNG FONT

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    isRunning = true;

    // Tải Font
    gameFont = TTF_OpenFont("font/arial.ttf", 32); // Tải font size 32
    if (!gameFont) {
        std::cout << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
    }

    backgroundMusic = Mix_LoadMUS("sound/music.mp3");
    if (backgroundMusic) {
        Mix_PlayMusic(backgroundMusic, -1);
        isMusicOn = true;
    }

    backgroundTexture = TextureManager::LoadTexture("image/background.png", renderer);
    pauseOverlay = TextureManager::LoadTexture("image/pause.png", renderer);
    map = new Map(renderer, "image/map.png");

    // <<-- TẢI CÁC TEXTURE GIAO DIỆN MỚI
    heartTexture = TextureManager::LoadTexture("image/heart.png", renderer);
    turn1Texture = TextureManager::LoadTexture("image/turn_player1.png", renderer);
    turn2Texture = TextureManager::LoadTexture("image/turn_player2.png", renderer);

    // <<-- TẢI TEXTURE MÀN HÌNH WIN VÀ NÚT CHƠI LẠI
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

// <<-- HÀM MỚI ĐỂ RESET GAME
void Game::resetGame() {
    // Dọn dẹp các đối tượng của ván chơi cũ
    delete player1;
    player1 = nullptr;
    delete player2;
    player2 = nullptr;

    for (auto p : projectiles) delete p;
    projectiles.clear();
    for (auto e : explosions) delete e;
    explosions.clear();

    // Reset các biến
    winner = 0;
    currentPlayerTurn = 1;

    // Quay về menu chính
    gameState = GameState::MAIN_MENU;
}

void Game::handleEvents() {
    // ... (Hàm này giữ nguyên như phiên bản trước) ...
    SDL_Event event;
    SDL_PollEvent(&event);

    if (event.type == SDL_QUIT) {
        isRunning = false;
    }
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
            if (player1Button->isClicked(&event)) {
                gameState = GameState::PLAYING_1P;
                player1 = new Player("image/nam.png", 100, GROUND_LEVEL_FOR_PROJECTILE - 64, renderer);
                player2 = new Player("image/nu.png", 600, GROUND_LEVEL_FOR_PROJECTILE - 64, renderer);
                switchTurn(); // Bắt đầu lượt đầu tiên
            }
            if (player2Button->isClicked(&event)) {
                gameState = GameState::PLAYING_2P;
                player1 = new Player("image/nam.png", 100, GROUND_LEVEL_FOR_PROJECTILE - 64, renderer);
                player2 = new Player("image/nu.png", 600, GROUND_LEVEL_FOR_PROJECTILE - 64, renderer);
                switchTurn(); // Bắt đầu lượt đầu tiên
            }
            if (settingsButton->isClicked(&event)) {
                gameState = GameState::SETTINGS;
            }
            if (exitButton->isClicked(&event)) {
                isRunning = false;
            }
            break;

        case GameState::SETTINGS:
            if (musicToggleButton->isClicked(&event)) {
                isMusicOn = !isMusicOn;
                if (isMusicOn) Mix_ResumeMusic(); else Mix_PauseMusic();
            }
            if (backButton->isClicked(&event)) {
                gameState = GameState::MAIN_MENU;
            }
            break;

        case GameState::GAME_OVER:
            if (playAgainButton->isClicked(&event)) {
                resetGame();
            }
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
    if (projectiles.empty()) {
        currentPlayerTurn = (currentPlayerTurn == 1) ? 2 : 1;
        turnStartTime = SDL_GetTicks();
        turnNotificationEndTime = SDL_GetTicks() + 2000; // <<-- HIỆN THÔNG BÁO TRONG 2 GIÂY
        std::cout << "\nPlayer " << currentPlayerTurn << "'s Turn" << std::endl;
    }
}

void Game::update() {
    if (gameState != GameState::PLAYING_1P && gameState != GameState::PLAYING_2P) return;

    player1->update();
    player2->update();

    // <<-- CẬP NHẬT ĐỒNG HỒ ĐẾM NGƯỢC
    Uint32 currentTime = SDL_GetTicks();
    int remainingTime = (TURN_DURATION - (currentTime - turnStartTime)) / 1000;
    if (remainingTime < 0) remainingTime = 0;
    createTextTexture(std::to_string(remainingTime)); // Tạo texture chữ mới

    //  update đạn, va chạm, hiệu ứng nổ giữ nguyên) ...
    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i]->update();
        SDL_Rect projRect = projectiles[i]->getRect();
        if (projRect.y + projRect.h >= GROUND_LEVEL_FOR_PROJECTILE) {
            explosions.push_back(new Explosion(renderer, projRect.x, projRect.y));
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            i--;
            switchTurn();
            continue;
        }
        Player* targetPlayer = (currentPlayerTurn == 1) ? player2 : player1;
        SDL_Rect originalRect = targetPlayer->getRect(); // Lấy hình chữ nhật 64x64 gốc

        // Tạo một hitbox nhỏ hơn, nằm bên trong hình chữ nhật gốc
        SDL_Rect targetHitbox = {
            originalRect.x + 15, // Lùi vào 15 pixel từ bên trái
            originalRect.y + 10, // Lùi vào 10 pixel từ phía trên
            34,                  // Chiều rộng mới = 64 - 15 (trái) - 15 (phải)
            54                   // Chiều cao mới = 64 - 10 (trên)
        };

        if (SDL_HasIntersection(&projRect, &targetHitbox)) {
            targetPlayer->health--;
            explosions.push_back(new Explosion(renderer, projRect.x, projRect.y));
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            i--;
            // <<-- CẬP NHẬT LOGIC CHIẾN THẮNG
            if (targetPlayer->health <= 0) {
                gameState = GameState::GAME_OVER;
                winner = currentPlayerTurn; // Lưu lại người chiến thắng
                std::cout << "GAME OVER! Player " << winner << " wins!" << std::endl;
            } else {
                switchTurn();
            }
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
        switchTurn();
    }
}

void Game::createTextTexture(const std::string& text) {
    if (timerTexture) {
        SDL_DestroyTexture(timerTexture); // Xóa texture cũ
        timerTexture = nullptr;
    }
    SDL_Color textColor = { 255, 255, 255, 255 }; // Màu trắng
    SDL_Surface* textSurface = TTF_RenderText_Solid(gameFont, text.c_str(), textColor);
    if (textSurface) {
        timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        timerRect = { (800 - textSurface->w) / 2, 10, textSurface->w, textSurface->h }; // Căn giữa trên
        SDL_FreeSurface(textSurface);
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (gameState == GameState::MAIN_MENU || gameState == GameState::SETTINGS) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        if (gameState == GameState::MAIN_MENU) {
            player1Button->render(renderer);
            player2Button->render(renderer);
            settingsButton->render(renderer);
            exitButton->render(renderer);
        } else {
            musicToggleButton->render(renderer);
            backButton->render(renderer);
        }
    }
    else if (gameState == GameState::GAME_OVER) {
        // Vẽ lại màn chơi cuối cùng
        if (map) map->render(renderer);
        if (player1) player1->render(renderer);
        if (player2) player2->render(renderer);

        // Vẽ ảnh người chiến thắng tương ứng
        SDL_Texture* winTexture = nullptr;
        if (winner == 1) winTexture = win1Texture;
        else if (winner == 2) winTexture = win2Texture;

        if (winTexture) {
            SDL_Rect winRect = {(800 - 400) / 2, (600 - 200) / 2, 400, 200}; // Căn giữa
            SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
        }

        // Vẽ nút chơi lại
        playAgainButton->render(renderer);
    }
    else {
        if (map) map->render(renderer);
        if (player1) player1->render(renderer);
        if (player2) player2->render(renderer);
        for (auto& p : projectiles) p->render(renderer);
        for (auto& e : explosions) e->render(renderer);

        // <<-- VẼ CÁC THÀNH PHẦN GIAO DIỆN (UI)
        // 1. Vẽ thanh máu
        if (player1 && heartTexture) {
            for (int i = 0; i < player1->health; ++i) {
                SDL_Rect heartRect = {10 + (i * 35), 10, 32, 32};
                SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
            }
        }
        if (player2 && heartTexture) {
            for (int i = 0; i < player2->health; ++i) {
                SDL_Rect heartRect = {790 - 32 - (i * 35), 10, 32, 32};
                SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
            }
        }
        // 2. Vẽ đồng hồ đếm ngược
        if (timerTexture) {
            SDL_RenderCopy(renderer, timerTexture, NULL, &timerRect);
        }
        // 3. Vẽ thông báo lượt chơi (nếu cần)
        if (SDL_GetTicks() < turnNotificationEndTime) {
            SDL_Texture* turnTexture = (currentPlayerTurn == 1) ? turn1Texture : turn2Texture;
            SDL_Rect turnRect = { (800 - 300) / 2, (600 - 100) / 2, 300, 100 }; // Căn giữa màn hình
            SDL_RenderCopy(renderer, turnTexture, NULL, &turnRect);
        }

        if (gameState == GameState::PAUSED && pauseOverlay) {
             SDL_RenderCopy(renderer, pauseOverlay, NULL, NULL);
        }
        // <<-- VẼ GIAO DIỆN HỖ TRỢ BẮN
        Player* currentPlayer = (currentPlayerTurn == 1) ? player1 : player2;
        if (currentPlayer) {
            // 1. Vẽ thanh lực (Power Bar)
            SDL_Rect powerBarBg = { currentPlayer->getRect().x, currentPlayer->getRect().y - 20, 100, 10 };
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Màu nền xám
            SDL_RenderFillRect(renderer, &powerBarBg);

            float powerPercent = currentPlayer->getPower() / MAX_POWER;
            SDL_Rect powerBarFg = { powerBarBg.x, powerBarBg.y, (int)(100 * powerPercent), 10 };
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255); // Màu lực vàng
            SDL_RenderFillRect(renderer, &powerBarFg);

            // 2. Vẽ đường ngắm (Aiming Line)
            float angle = currentPlayer->getAngle();
            bool facingRight = currentPlayer->isFacingRight();
            float shootingAngle = facingRight ? angle : 180.0f - angle;
            float angleRad = shootingAngle * PI / 180.0f;

            int startX = currentPlayer->getRect().x + currentPlayer->getRect().w / 2;
            int startY = currentPlayer->getRect().y + currentPlayer->getRect().h / 2;
            int endX = startX + (int)(cos(angleRad) * 40); // Độ dài 40 pixel
            int endY = startY - (int)(sin(angleRad) * 40); // Dấu trừ vì trục Y ngược

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Màu trắng
            SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
        }

        if (gameState == GameState::PAUSED && pauseOverlay) {
            SDL_RenderCopy(renderer, pauseOverlay, NULL, NULL);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // ... (Giải phóng các tài nguyên cũ giữ nguyên)
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(pauseOverlay);
    delete player1Button;
    delete player2Button;
    delete settingsButton;
    delete exitButton;
    delete musicToggleButton;
    delete backButton;
    delete player1;
    delete player2;
    delete map;

    // <<-- GIẢI PHÓNG CÁC TÀI NGUYÊN MỚI
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(turn1Texture);
    SDL_DestroyTexture(turn2Texture);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(win1Texture);
    SDL_DestroyTexture(win2Texture);
    delete playAgainButton;
    TTF_CloseFont(gameFont);

    for (auto p : projectiles) delete p;
    projectiles.clear();
    for (auto e : explosions) delete e;
    explosions.clear();

    Mix_FreeMusic(backgroundMusic);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    // Đóng các hệ thống
    Mix_CloseAudio();
    TTF_Quit(); // <<-- ĐÓNG HỆ THỐNG FONT
    IMG_Quit();
    SDL_Quit();
}