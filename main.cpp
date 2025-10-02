// main.cpp

// 1. Bao gồm các thư viện cần thiết
#include <SDL.h>
#include <iostream>
#include <SDL_image.h> // Thêm thư viện SDL_image
#include <string>

// 2. Định nghĩa các hằng số cho cửa sổ
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Hàm để tải một ảnh và tạo texture
SDL_Texture* loadTexture(const std::string &path, SDL_Renderer* renderer) {
    // Texture cuối cùng sẽ được trả về
    SDL_Texture* newTexture = nullptr;

    // Tải ảnh từ đường dẫn
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cout << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        // Tạo texture từ surface
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            std::cout << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        }

        // Giải phóng surface cũ vì đã có texture
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}


// Hàm main - điểm bắt đầu của chương trình
int main(int argc, char* args[]) {

    // 3. Khai báo con trỏ cho cửa sổ, renderer và texture
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* backgroundTexture = nullptr; // Con trỏ cho texture ảnh nền

    // 4. Khởi tạo SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // *** THÊM MỚI: Khởi tạo SDL_image để tải ảnh PNG ***
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 5. Tạo cửa sổ
    window = SDL_CreateWindow(
        "Gunny SDL2 - Giai Doan 1",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit(); // Dọn dẹp SDL_image
        SDL_Quit();
        return -1;
    }

    // 6. Tạo renderer để vẽ lên cửa sổ
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // *** THÊM MỚI: Tải ảnh nền ***
    backgroundTexture = loadTexture("image/background.png", renderer);
    if (backgroundTexture == nullptr) {
        // Lỗi đã được in trong hàm loadTexture, chỉ cần dọn dẹp và thoát
        std::cout << "Press Enter to exit..." << std::endl; // Thêm dòng này
        std::cin.get();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }


    // 7. Vòng lặp game chính (Game Loop)
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        // 8. Xử lý hàng đợi sự kiện
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // 9. Logic vẽ (Rendering)
        // Xóa toàn bộ màn hình (bước này vẫn cần thiết để đảm bảo không có dư ảnh từ frame trước)
        SDL_RenderClear(renderer);

        // *** THÊM MỚI: Vẽ ảnh nền lên toàn bộ màn hình ***
        // NULL cho destination rect có nghĩa là vẽ texture lên toàn bộ renderer (cửa sổ)
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // (Ở các giai đoạn sau, chúng ta sẽ vẽ các đối tượng khác lên trên background)

        // 10. Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // 11. Dọn dẹp tài nguyên và thoát
    SDL_DestroyTexture(backgroundTexture); // *** THÊM MỚI: Giải phóng texture nền ***
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); // *** THÊM MỚI: Dọn dẹp SDL_image ***
    SDL_Quit();

    // Gán lại con trỏ thành NULL để tránh sử dụng nhầm
    backgroundTexture = nullptr;
    renderer = nullptr;
    window = nullptr;

    return 0;
}