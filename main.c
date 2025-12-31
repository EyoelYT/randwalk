#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <time.h>

typedef struct {
    int r;
    int g;
    int b;
    int a;
} RGB;

// RGB hslToRgb(float h, float s, float l) {
//     float r, g, b;
//     if (s == 0.0) {
//         r = g = b = l; // achromatic
//     } else {
//         float q = (l < 0.5) ? (l * (1 + s)) : (l + s - l * s);
//         float p = 2 * l - q;
//         #define HUE_TO_RGB(v) \
//             (v < 0.0 ? v + 1.0 : (v > 1.0 ? v - 1.0 : v)); \
//             if (v < 1.0/6.0) r = p + (q - p) * 6 * v; \
//             else if (v < 1.0/2.0) r = q; \
//             else if (v < 2.0/3.0) r = p + (q - p) * (2.0/3.0 - v) * 6; \
//             else r = p;

//         float h_norm = h / 360.0; // Normalize hue to 0-1 range
//         float v_r = h_norm + 1.0/3.0;
//         float v_g = h_norm;
//         float v_b = h_norm - 1.0/3.0;

//         // Manually implement HUE_TO_RGB for all channels to avoid macro issues
//         // Red
//         if (v_r < 0.0) v_r += 1.0;
//         if (v_r > 1.0) v_r -= 1.0;
//         if (v_r < 1.0/6.0) r = p + (q - p) * 6 * v_r;
//         else if (v_r < 1.0/2.0) r = q;
//         else if (v_r < 2.0/3.0) r = p + (q - p)* (2.0/3.0 - v_r) * 6;
//         else r = p;
        
//         // Green
//         if (v_g < 0.0) v_g += 1.0;
//         if (v_g > 1.0) v_g -= 1.0;
//         if (v_g < 1.0/6.0) g = p + (q - p) * 6 * v_g;
//         else if (v_g < 1.0/2.0) g = q;
//         else if (v_g < 2.0/3.0) g = p + (q - p) * (2.0/3.0 - v_g) * 6;
//         else g = p;

//         // Blue
//         if (v_b < 0.0) v_b += 1.0;
//         if (v_b > 1.0) v_b -= 1.0;
//         if (v_b < 1.0/6.0) b = p + (q - p) * 6 * v_b;
//         else if (v_b < 1.0/2.0) b = q;
//         else if (v_b < 2.0/3.0) b = p + (q - p) * (2.0/3.0 - v_b) * 6;
//         else b = p;
//         #undef HUE_TO_RGB
//     }

//     RGB color;
//     color.r = (unsigned char)(r * 255.0);
//     color.g = (unsigned char)(g * 255.0);
//     color.b = (unsigned char)(b * 255.0);
//     color.a = 0;
//     return color;
// }

RGB rand_rgb() {
    // // Generate a random hue (0 to 360 degrees)
    // float hue = (float)(rand() % 360);

    // // Set saturation and lightness to high values for brightness
    // float saturation = 0.5 + (float)(rand() % 10) / 100.0; // 0.9 to 1.0
    // float lightness = 0.5 + (float)(rand() % 50) / 100.0; // 0.5 to 1.0

    // // Convert HSL to RGB
    // return hslToRgb(hue, saturation, lightness);

    RGB color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 0;
    return color;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *pwindow = SDL_CreateWindow(
        "RandWalk",           // Window title
        SDL_WINDOWPOS_CENTERED,   // Initial x position (centered)
        SDL_WINDOWPOS_CENTERED,   // Initial y position (centered)
        800,                      // Width in pixels
        600,                      // Height in pixels
        SDL_WINDOW_SHOWN      // Flags (make it visible)
    );

    SDL_Renderer* prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_SOFTWARE);

    bool quit = false;
    SDL_Event event;
    SDL_Rect rect = {.x = 2, .y = 2, .w = 2, .h = 2};

    RGB rgb_box = rand_rgb();
    SDL_SetRenderDrawColor(prenderer, rgb_box.r, rgb_box.g, rgb_box.b, rgb_box.a);
    SDL_RenderFillRect(prenderer, &rect);
    SDL_RenderPresent(prenderer);
    
    int x_direction = 5;
    int y_direction = 5;

    while (!quit) {
        // Poll for events
        while (SDL_PollEvent(&event) != 0) {
            // User requests quit
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        int window_width;
        int window_height;
        SDL_GetWindowSize(pwindow, &window_width, &window_height);
        if (rect.x + rect.w >= window_width) {
            x_direction = -x_direction;
            rgb_box = rand_rgb();
        } else if (rect.x <= 0) {
            x_direction = abs(x_direction);
            rgb_box = rand_rgb();
        }
        if (rect.y + rect.h >= window_height) {
            y_direction = -y_direction;
            rgb_box = rand_rgb();
        } else if (rect.y <= 0) {
            y_direction = abs(y_direction);
            rgb_box = rand_rgb();
        }
        // printf("x = %d\n", rect.x);
        // printf("y = %d\n", rect.y);
        // printf("\n");
        rect.x += x_direction;
        rect.y += y_direction;

        SDL_SetRenderDrawColor(prenderer, rgb_box.r, rgb_box.g, rgb_box.g, rgb_box.a );
        SDL_RenderFillRect(prenderer, &rect);
        SDL_RenderPresent(prenderer);

        // SDL_Delay(10);
    }
}