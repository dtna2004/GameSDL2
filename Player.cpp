#include "Player.h"
#include "TextureManager.h"
#include <iostream>

const float PLAYER_GRAVITY = 0.5f;
const float PLAYER_MOVE_SPEED = 3.0f; // tốc độ di chuyển

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
    if (!projectiles.empty()) {
        x_vel = 0; // Nếu đang bắn thì không cho di chuyển
        return;
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    // THAY ĐỔI LOGIC: Thay vì trực tiếp thay đổi rect.x, chúng ta sẽ đặt vận tốc
    x_vel = 0; // Reset vận tốc ở đầu mỗi frame
    if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
        x_vel = -PLAYER_MOVE_SPEED;
        facingRight = false;
    }
    if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
        x_vel = PLAYER_MOVE_SPEED;
        facingRight = true;
    }

    // Logic ngắm bắn và khai hỏa (giữ nguyên)
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

// HÀM UPDATE ĐỂ XỬ LÝ VA CHẠM
void Player::update(Map* map) {
    if (isCharging) {
        power += 5.0f;
        if (power > 500.0f) power = 500.0f;
    }

    // === 1. XỬ LÝ VA CHẠM NGANG ===
    rect.x += (int)x_vel;
    SDL_Rect playerHitbox = { rect.x + 16, rect.y, 32, rect.h - 4 }; // Hitbox hẹp và thấp hơn một chút

    // Nếu đang đi sang phải
    if (x_vel > 0) {
        int rightSide = playerHitbox.x + playerHitbox.w;
        int tileCol = rightSide / TILE_SIZE;
        if (map->getTileTypeFromPixelCoords(rightSide, playerHitbox.y) != 0 ||
            map->getTileTypeFromPixelCoords(rightSide, playerHitbox.y + playerHitbox.h) != 0)
        {
            // Nếu va chạm, "đẩy" người chơi lùi lại để mép phải của họ chạm vào mép trái của ô gạch
            rect.x = tileCol * TILE_SIZE - (16 + 32); // 16 là offset, 32 là width của hitbox
        }
    }
    // Nếu đang đi sang trái
    else if (x_vel < 0) {
        int leftSide = playerHitbox.x;
        int tileCol = leftSide / TILE_SIZE;
         if (map->getTileTypeFromPixelCoords(leftSide, playerHitbox.y) != 0 ||
            map->getTileTypeFromPixelCoords(leftSide, playerHitbox.y + playerHitbox.h) != 0)
        {
            // Nếu va chạm, đẩy người chơi sang phải để mép trái của họ chạm vào mép phải của ô gạch
            rect.x = (tileCol + 1) * TILE_SIZE - 16; // 16 là offset
        }
    }


    // === 2. XỬ LÝ VA CHẠM DỌC (TRỌNG LỰC) ===
    y_vel += PLAYER_GRAVITY;
    rect.y += (int)y_vel;
    playerHitbox = { rect.x + 16, rect.y, 32, rect.h }; // Cập nhật lại hitbox với vị trí Y mới

    // Nếu đang rơi xuống
    if (y_vel > 0) {
        int footY = playerHitbox.y + playerHitbox.h;
        if (map->getTileTypeFromPixelCoords(playerHitbox.x, footY) != 0 ||
            map->getTileTypeFromPixelCoords(playerHitbox.x + playerHitbox.w, footY) != 0)
        {
            rect.y = ( footY / TILE_SIZE ) * TILE_SIZE - rect.h;
            onGround = true;
            y_vel = 0;
        } else {
            onGround = false;
        }
    }
    // Nếu đang bay lên(sau này phát triển thêm máy bay dịch chuyển vị trí :))
    else if (y_vel < 0) {
        if (map->getTileTypeFromPixelCoords(playerHitbox.x, playerHitbox.y) != 0 ||
            map->getTileTypeFromPixelCoords(playerHitbox.x + playerHitbox.w, playerHitbox.y) != 0)
        {
            rect.y = ( (playerHitbox.y / TILE_SIZE) + 1 ) * TILE_SIZE;
            y_vel = 0;
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