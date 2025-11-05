#include "Game.h"

Game *game = nullptr;

int main(int argc, char *argv[]) {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    //đánh dấu thời gian bắt đầu khung hình
    Uint32 frameStart;
    //thời gian thực tế xử lí 1 khung hình
    int frameTime;

    game = new Game();
    game->init("Gunny SDL", 800, 600);

    while (game->running()) {
        frameStart = SDL_GetTicks();
        //lắng nghe sự kiện
        game->handleEvents();
        //cập nhật các sự kiện
        game->update();
        //render màn theo sự kiện
        game->render();

        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    game->clean();
    delete game;

    return 0;
}