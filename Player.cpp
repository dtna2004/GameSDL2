#include "Player.h"
#include "TextureManager.h"
#include <iostream>

// Định nghĩa các hằng số cho dễ quản lý
const float PLAYER_GRAVITY = 0.5f;
const int GROUND_LEVEL = 480; // MẶT ĐẤT ẢO SẼ Ở TỌA ĐỘ Y = 480

Player::Player(const std::string& texturePath, int x, int y, SDL_Renderer* renderer) {
    texture = TextureManager::LoadTexture(texturePath, renderer);
    rect = { x, y, 64, 64 };
    if (x > 400) {
        facingRight = false;
    }
}

Player::~Player() {
    SDL_DestroyTexture(texture);
}

void Player::handleEvent(SDL_Event& e, std::vector<Projectile*>& projectiles, SDL_Renderer* renderer) {
    // ... (Toàn bộ nội dung hàm này giữ nguyên không đổi) ...
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
        if (angle > 90) angle = 90;
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
            float shootingAngle = facingRight ? angle : 180.0f - angle;
            projectiles.push_back(new Projectile(renderer, rect.x + 32, rect.y + 32, shootingAngle, power));
            std::cout << "FIRE! Angle: " << shootingAngle << ", Power: " << power << std::endl;
        }
    }
}

// HÀM UPDATE ĐƯỢC ĐƠN GIẢN HÓA
void Player::update() {
    // 1. Cập nhật trạng thái tăng lực
    if (isCharging) {
        power += 5.0f;
        if (power > 500.0f) {
            power = 500.0f;
        }
    }

    // 2. Áp dụng trọng lực nếu không ở trên mặt đất
    if (!onGround) {
        y_vel += PLAYER_GRAVITY;
        rect.y += (int)y_vel;
    }

    // 3. Kiểm tra va chạm với MẶT ĐẤT ẢO
    if (rect.y + rect.h >= GROUND_LEVEL) {
        rect.y = GROUND_LEVEL - rect.h; // Đặt nhân vật đứng ngay trên mặt đất
        y_vel = 0;
        onGround = true;
    } else {
        onGround = false;
    }
}

void Player::render(SDL_Renderer* renderer) {
    // ... (Hàm này giữ nguyên không đổi) ...
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0.0, NULL, flip);
}

SDL_Rect Player::getRect() {
    // ... (Hàm này giữ nguyên không đổi) ...
    return rect;
}