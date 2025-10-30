#include "Player.h"
#include "TextureManager.h"
#include <iostream>

// Định nghĩa các hằng số cho vật lý của người chơi
const float PLAYER_GRAVITY = 0.5f;
const float PLAYER_MOVE_SPEED = 2.0f;

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
    if (!projectiles.empty()) return;

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    // Di chuyển ngang (Logic va chạm ngang được tích hợp ở đây)
    // Phần này được chuyển từ update() sang đây để chỉ di chuyển khi có input
    if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
        rect.x -= PLAYER_MOVE_SPEED;
        facingRight = false;
    }
    if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
        rect.x += PLAYER_MOVE_SPEED;
        facingRight = true;
    }


    // Logic ngắm bắn và khai hỏa (giữ nguyên không đổi)
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
        }
    }
    if (e.type == SDL_KEYUP && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN)) {
        if (isCharging) {
            isCharging = false;
            float shootingAngle = facingRight ? angle : 180.0f - angle;
            projectiles.push_back(new Projectile(renderer, rect.x + 32, rect.y + 32, shootingAngle, power));
        }
    }
}

void Player::update(Map* map) {
    if (isCharging) {
        power += 5.0f;
        if (power > 500.0f) power = 500.0f;
    }

    // === XỬ LÝ VẬT LÝ VÀ VA CHẠM DỌC (TRỌNG LỰC) ===
    y_vel += PLAYER_GRAVITY;
    rect.y += (int)y_vel;

    // Tạo một hitbox hẹp hơn để nhân vật có thể đi qua các khe hở hẹp
    SDL_Rect playerHitbox = { rect.x + 16, rect.y, 32, rect.h };

    // Kiểm tra va chạm với đất (khi đang rơi xuống)
    if (y_vel > 0) {
        int footLeftTile = map->getTileTypeFromPixelCoords(playerHitbox.x, playerHitbox.y + playerHitbox.h);
        int footRightTile = map->getTileTypeFromPixelCoords(playerHitbox.x + playerHitbox.w, playerHitbox.y + playerHitbox.h);
        if (footLeftTile != 0 || footRightTile != 0) {
            // Nếu va chạm, "đặt" người chơi ngay trên mép trên của ô gạch
            rect.y = ( (playerHitbox.y + playerHitbox.h) / TILE_SIZE ) * TILE_SIZE - rect.h;
            onGround = true;
            y_vel = 0;
        } else {
            onGround = false;
        }
    }
    // Kiểm tra va chạm với trần (khi đang nhảy lên)
    else if (y_vel < 0) {
        int headLeftTile = map->getTileTypeFromPixelCoords(playerHitbox.x, playerHitbox.y);
        int headRightTile = map->getTileTypeFromPixelCoords(playerHitbox.x + playerHitbox.w, playerHitbox.y);
        if (headLeftTile != 0 || headRightTile != 0) {
            // Nếu va chạm, "đặt" người chơi ngay dưới mép dưới của ô gạch
            rect.y = ( (playerHitbox.y / TILE_SIZE) + 1 ) * TILE_SIZE;
            y_vel = 0; // Dừng việc bay lên
        }
    }
}

void Player::render(SDL_Renderer* renderer) {
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0.0, NULL, flip);
}

SDL_Rect Player::getRect() {
    return rect;
}

bool Player::isOutOfBound(int screenHeight) const {
    return rect.y > screenHeight;
}