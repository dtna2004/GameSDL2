#include "Game.h"
#include "TextureManager.h"
#include <iostream>

// Constructor và Destructor để trống
Game::Game() {}
Game::~Game() {}

void Game::init(const char* title, int width, int height) {
    // Khởi tạo SDL Video và Audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Khởi tạo SDL_image
    if (!IMG_Init(IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        isRunning = false;
        return;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        // Không thoát game nếu chỉ có âm thanh lỗi, nhưng sẽ không có nhạc
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    isRunning = true;

    // Tải và chơi nhạc nền
    backgroundMusic = Mix_LoadMUS("sound/music.mp3");
    if (!backgroundMusic) {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    } else {
        Mix_PlayMusic(backgroundMusic, -1); // Tham số -1 để lặp vô tận
    }

    // Tải các texture dùng chung
    backgroundTexture = TextureManager::LoadTexture("image/background.png", renderer);
    pauseOverlay = TextureManager::LoadTexture("image/pause.png", renderer);

    // Tạo các nút cho menu chính
    player1Button = new Button("image/1_player_button.png", 100, 350, 250, 80, renderer);
    player2Button = new Button("image/2_player_button.png", 450, 350, 250, 80, renderer);
    settingsButton = new Button("image/settings_button.png", 100, 450, 250, 80, renderer);
    exitButton = new Button("image/exit_button.png", 450, 450, 250, 80, renderer);

    // Bắt đầu game ở trạng thái Main Menu
    gameState = GameState::MAIN_MENU;
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);

    // Xử lý sự kiện chung
    if (event.type == SDL_QUIT) {
        isRunning = false;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p && !event.key.repeat) {
        if (gameState == GameState::PLAYING_1P || gameState == GameState::PLAYING_2P) {
            stateBeforePause = gameState; // Lưu lại trạng thái
            gameState = GameState::PAUSED;
            Mix_PauseMusic(); // Dừng nhạc
            std::cout << "Game Paused" << std::endl;
        } else if (gameState == GameState::PAUSED) {
            gameState = stateBeforePause; // Khôi phục trạng thái
            Mix_ResumeMusic(); // Tiếp tục chơi nhạc
            std::cout << "Game Resumed" << std::endl;
        }
    }

    // Xử lý sự kiện theo từng trạng thái game
    switch (gameState) {
        case GameState::MAIN_MENU:
            if (player1Button->isClicked(&event)) {
                gameState = GameState::PLAYING_1P;
                map = new Map(renderer, "image/map.png");
                player1 = new Player("image/male.png", 100, 150, renderer);
                player2 = new Player("image/female.png", 600, 150, renderer); // Sẽ là AI
                currentPlayerTurn = 1;
                turnStartTime = SDL_GetTicks();
            }
            if (player2Button->isClicked(&event)) {
                gameState = GameState::PLAYING_2P;
                map = new Map(renderer, "image/map.png");
                player1 = new Player("image/male.png", 100, 150, renderer);
                player2 = new Player("image/female.png", 600, 150, renderer);
                currentPlayerTurn = 1;
                turnStartTime = SDL_GetTicks();
            }
            if (settingsButton->isClicked(&event)) {
                std::cout << "Settings clicked (not implemented yet)" << std::endl;
            }
            if (exitButton->isClicked(&event)) {
                isRunning = false;
            }
            break;

        case GameState::PLAYING_1P: // Tạm thời cho người chơi điều khiển cả 2
        case GameState::PLAYING_2P:
            if (currentPlayerTurn == 1) {
                player1->handleEvent(event, projectiles, renderer);
            } else {
                player2->handleEvent(event, projectiles, renderer);
            }
            break;

        default:
            break;
    }
}

void Game::switchTurn() {
    if (projectiles.empty()) {
        currentPlayerTurn = (currentPlayerTurn == 1) ? 2 : 1;
        turnStartTime = SDL_GetTicks();
        std::cout << "\nPlayer " << currentPlayerTurn << "'s Turn" << std::endl;
    }
}

void Game::update() {
    // Chỉ cập nhật logic game khi đang ở trạng thái chơi
    if (gameState != GameState::PLAYING_1P && gameState != GameState::PLAYING_2P) {
        return;
    }

    // Cập nhật người chơi (trọng lực, va chạm đất)
    player1->update(map);
    player2->update(map);

    // Cập nhật đạn và kiểm tra va chạm
    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i]->update();
        SDL_Rect projRect = projectiles[i]->getRect();

        // Va chạm với map hoặc ra khỏi màn hình
        if (map->isSolidAt(projRect.x + projRect.w / 2, projRect.y + projRect.h / 2) || projRect.y > 600) {
            explosions.push_back(new Explosion(renderer, projRect.x, projRect.y));
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            i--; // Quan trọng: vì đã xóa 1 phần tử
            switchTurn();
            continue;
        }

        // Va chạm với người chơi mục tiêu
        Player* targetPlayer = (currentPlayerTurn == 1) ? player2 : player1;
        SDL_Rect targetRect = targetPlayer->getRect();
        if (SDL_HasIntersection(&projRect, &targetRect)) {
            targetPlayer->health--;
            std::cout << "HIT! Player " << ((currentPlayerTurn == 1) ? 2 : 1) << " health: " << targetPlayer->health << std::endl;
            explosions.push_back(new Explosion(renderer, projRect.x, projRect.y));
            delete projectiles[i];
            projectiles.erase(projectiles.begin() + i);
            i--;

            if (targetPlayer->health <= 0) {
                gameState = GameState::GAME_OVER;
                std::cout << "GAME OVER! Player " << currentPlayerTurn << " wins!" << std::endl;
            } else {
                switchTurn();
            }
        }
    }

    // Cập nhật hiệu ứng nổ
    for (int i = 0; i < explosions.size(); ++i) {
        explosions[i]->update();
        if (explosions[i]->isFinished()) {
            delete explosions[i];
            explosions.erase(explosions.begin() + i);
            i--;
        }
    }

    // Hết giờ thì chuyển lượt
    if (projectiles.empty() && SDL_GetTicks() > turnStartTime + TURN_DURATION) {
        std::cout << "Time's up!" << std::endl;
        switchTurn();
    }
}

void Game::render() {
    SDL_RenderClear(renderer);

    if (gameState == GameState::MAIN_MENU) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        player1Button->render(renderer);
        player2Button->render(renderer);
        settingsButton->render(renderer);
        exitButton->render(renderer);
    } else {
        // Vẽ màn hình game cho tất cả các trạng thái còn lại (PLAYING, PAUSED, GAME_OVER)
        if (map) map->render(renderer);
        if (player1) player1->render(renderer);
        if (player2) player2->render(renderer);
        for (auto& p : projectiles) {
            p->render(renderer);
        }
        for (auto& e : explosions) {
            e->render(renderer);
        }

        // Nếu đang PAUSE, vẽ thêm lớp phủ mờ lên trên
        if (gameState == GameState::PAUSED && pauseOverlay) {
             SDL_SetTextureBlendMode(pauseOverlay, SDL_BLENDMODE_BLEND);
             SDL_SetTextureAlphaMod(pauseOverlay, 150); // Đặt độ trong suốt
             SDL_RenderCopy(renderer, pauseOverlay, NULL, NULL);
        }

        if (gameState == GameState::GAME_OVER){
            // Bạn có thể vẽ chữ "GAME OVER" ở đây
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Giải phóng tài nguyên theo thứ tự ngược lại với khi tạo
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(pauseOverlay);
    delete player1Button;
    delete player2Button;
    delete settingsButton;
    delete exitButton;
    delete player1;
    delete player2;
    delete map;

    for (auto p : projectiles) delete p;
    projectiles.clear();
    for (auto e : explosions) delete e;
    explosions.clear();

    Mix_FreeMusic(backgroundMusic);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game Cleaned Successfully" << std::endl;
}