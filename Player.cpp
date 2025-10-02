#include "Player.h"
#include "TextureManager.h"
#include <iostream>

const float GRAVITY = 0.5f;

Player::Player(const std::string& texturePath, int x, int y, SDL_Renderer* renderer) {
    texture = TextureManager::LoadTexture(texturePath, renderer);
    rect = { x, y, 64, 64 };
    if (x > 400) { // Nếu người chơi ở nửa phải màn hình, mặc định quay sang trái
        facingRight = false;
    }
}

Player::~Player() {
    SDL_DestroyTexture(texture);
}

void Player::handleEvent(SDL_Event& e, std::vector<Projectile*>& projectiles, SDL_Renderer* renderer) {
    if (!projectiles.empty()) return;

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
        rect.x -= 2;
        facingRight = false;
    }
    if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
        rect.x += 2;
        facingRight = true;
    }
    if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_PAGEUP]) {
        angle += 1.0f;
        if (angle > 90) angle = 90; // Giới hạn góc bắn từ 0-90 độ
    }
    if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_PAGEDOWN]) {
        angle -= 1.0f;
        if (angle < 0) angle = 0;
    }

    if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) && !e.key.repeat) {
        if (!isCharging) {
             isCharging = true;
             power = 0.0f;
             std::cout << "Charging..." << std::endl;
        }
    }
    if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN)) {
        if (isCharging) {
            isCharging = false;
            // Tính góc bắn thực tế dựa trên hướng nhìn
            float shootingAngle = facingRight ? angle : 180.0f - angle;
            projectiles.push_back(new Projectile(renderer, rect.x + 32, rect.y + 32, shootingAngle, power));
            std::cout << "FIRE! Angle: " << shootingAngle << ", Power: " << power << std::endl;
        }
    }
}

void Player::update(Map* map) {
    // 1. Cập nhật trạng thái tăng lực
    if (isCharging) {
        power += 5.0f;
        if (power > 500.0f) {
            power = 500.0f;
        }
    }

    // 2. Áp dụng trọng lực
    if (!onGround) {
        y_vel += GRAVITY;
        rect.y += (int)y_vel;
    }

    // 3. Kiểm tra va chạm với map
    onGround = false;
    // Kiểm tra điểm dưới chân nhân vật
    int footX = rect.x + rect.w / 2;
    int footY = rect.y + rect.h;

    while (map->isSolidAt(footX, footY) && footY > 0) {
        rect.y--; // Nếu đang ở trong đất, đẩy nhân vật lên
        footY = rect.y + rect.h;
        y_vel = 0; // Reset vận tốc rơi
        onGround = true;
    }
}

void Player::render(SDL_Renderer* renderer) {
    // Dùng SDL_RenderCopyEx để có thể lật hình nhân vật
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0.0, NULL, flip);
}

SDL_Rect Player::getRect() {
    return rect;
}