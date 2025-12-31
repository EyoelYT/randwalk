#include <unistd.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *pwindow = SDL_CreateWindow(
        "RandWalk",           // Window title
        SDL_WINDOWPOS_CENTERED,   // Initial x position (centered)
        SDL_WINDOWPOS_CENTERED,   // Initial y position (centered)
        800,                      // Width in pixels
        600,                      // Height in pixels
        SDL_WINDOW_SHOWN      // Flags (make it visible)
    );

    SDL_Renderer *prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_SOFTWARE);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255);

    // Clear winow
    SDL_RenderClear(prenderer);

    bool quit = false;
    SDL_Event event;

    SDL_Rect rect = {.x = 5, .y = 5, .w = 5, .h = 5};

    SDL_SetRenderDrawColor(prenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(prenderer, &rect);
    SDL_RenderPresent(prenderer);

    while (!quit) {
        // Poll for events
        while (SDL_PollEvent(&event) != 0) {
            // User requests quit
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        // You would put rendering code here in a real application (e.g., SDL_RenderClear, SDL_RenderPresent).
        rect.x += 5;
        rect.y += 5;

        SDL_SetRenderDrawColor(prenderer, 0, 0, 255, 255 );
        SDL_RenderFillRect(prenderer, &rect);
        SDL_RenderPresent(prenderer);

        SDL_Delay(10);
    }
}