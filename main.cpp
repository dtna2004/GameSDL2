// main.cpp

// 1. Bao gồm thư viện chính của SDL2
#include <SDL.h>
#include <iostream> // Dùng để in lỗi ra console nếu có

#include <string>

// 2. Định nghĩa các hằng số cho cửa sổ
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Hàm main - điểm bắt đầu của chương trình
int main(int argc, char* args[]) {

    // 3. Khai báo con trỏ cho cửa sổ và renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // 4. Khởi tạo SDL Video
    // SDL_Init trả về giá trị âm nếu có lỗi
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // In lỗi ra console
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1; // Thoát chương trình với mã lỗi
    }

    // 5. Tạo cửa sổ
    window = SDL_CreateWindow(
        "Gunny SDL2 - Giai Doan 1",     // Tiêu đề cửa sổ
        SDL_WINDOWPOS_UNDEFINED,      // Vị trí x ban đầu
        SDL_WINDOWPOS_UNDEFINED,      // Vị trí y ban đầu
        SCREEN_WIDTH,                 // Chiều rộng
        SCREEN_HEIGHT,                // Chiều cao
        SDL_WINDOW_SHOWN              // Cờ để hiển thị cửa sổ
    );

    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit(); // Dọn dẹp SDL trước khi thoát
        return -1;
    }

    // 6. Tạo renderer để vẽ lên cửa sổ
    // -1: dùng driver render đầu tiên hỗ trợ
    // SDL_RENDERER_ACCELERATED: dùng tăng tốc phần cứng
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); // Dọn dẹp cửa sổ
        SDL_Quit();
        return -1;
    }

    // 7. Vòng lặp game chính (Game Loop)
    bool quit = false; // Biến điều khiển vòng lặp
    SDL_Event e;       // Biến để lưu trữ sự kiện

    while (!quit) {
        // 8. Xử lý hàng đợi sự kiện
        while (SDL_PollEvent(&e) != 0) {
            // Nếu người dùng nhấn nút 'X' trên cửa sổ
            if (e.type == SDL_QUIT) {
                quit = true; // Đặt biến quit thành true để thoát vòng lặp
            }
        }

        // 9. Logic vẽ (Rendering)
        // Đặt màu vẽ cho renderer (R, G, B, A) - màu xanh da trời
        SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
        // Xóa toàn bộ màn hình với màu đã chọn
        SDL_RenderClear(renderer);

        // (Ở các giai đoạn sau, chúng ta sẽ vẽ mọi thứ ở đây)

        // 10. Cập nhật màn hình
        // Hiển thị những gì renderer đã vẽ lên cửa sổ
        SDL_RenderPresent(renderer);
    }

    // 11. Dọn dẹp tài nguyên và thoát
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Gán lại con trỏ thành NULL để tránh sử dụng nhầm
    renderer = nullptr;
    window = nullptr;

    return 0;
}