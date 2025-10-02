cmake_minimum_required(VERSION 3.15)
project(GameSDL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Chỉ cho CMake biết nơi tìm các tệp cấu hình của SDL2 (SDL2Config.cmake)
set(SDL2_DIR "E:/GameSDL/SDL2-devel-2.28.5-mingw/SDL2-2.28.5/x86_64-w64-mingw32/lib/cmake/SDL2")
set(SDL2_image_DIR "E:/GameSDL/SDL2_image-devel-2.8.8-mingw/SDL2_image-2.8.8/x86_64-w64-mingw32/lib/cmake/SDL2_image")
# Tìm gói SDL2. Thao tác này sẽ tạo ra các target như SDL2::SDL2 và SDL2::SDL2main
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Thêm tệp thực thi của bạn
add_executable(GameSDL main.cpp)

# --- ĐÂY LÀ DÒNG SỬA LỖI QUAN TRỌNG NHẤT ---
#
# Liên kết với CẢ HAI thư viện:
# 1. SDL2::SDL2main - Cung cấp điểm vào WinMain() để gọi main() của bạn.
# 2. SDL2::SDL2     - Cung cấp tất cả các chức năng còn lại của SDL.
#
target_link_libraries(GameSDL PRIVATE SDL2_image::SDL2_image SDL2::SDL2main SDL2::SDL2 )