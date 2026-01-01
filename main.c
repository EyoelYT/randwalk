#include <SDL.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

const int width = 800;
const int height = 600;
const double PI = 3.14159265358979323846;

typedef struct {
    int r;
    int g;
    int b;
    int a;
} RGB;

typedef struct {
    int x;
    int y;
} Vec2;

typedef struct {
    SDL_Rect rect;
    Vec2 velocity;
    RGB rgb;
    double degree;
    double degree_direction;
} BouncingRect;

typedef struct {
    int window_right_bound;
    int window_bottom_bound;
    int window_left_bound;
    int window_top_bound;
} WindowBounds;

RGB rand_rgb() {
    RGB color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = rand() % 256;
    return color;
}

Vec2 get_spiral_point(double degree) {
    double theta_radian = degree * (PI / 180.0);
    double k = 1.5;
    double radius = k * theta_radian;

    double x = radius * cos(theta_radian);
    double y = radius * sin(theta_radian);
    Vec2 point = { x, y };
    return point;
}

void update_bouncing_rect(BouncingRect *bouncing_rect, const WindowBounds window_bounds) {
    if (bouncing_rect->rect.x + bouncing_rect->rect.w >=
        window_bounds.window_right_bound) {
        bouncing_rect->velocity.x = -bouncing_rect->velocity.x;
        bouncing_rect->rgb = rand_rgb();
    } else if (bouncing_rect->rect.x <= 0) {
        bouncing_rect->velocity.x = abs(bouncing_rect->velocity.x);
        bouncing_rect->rgb = rand_rgb();
    }
    if (bouncing_rect->rect.y + bouncing_rect->rect.h >=
        window_bounds.window_bottom_bound) {
        bouncing_rect->velocity.y = -bouncing_rect->velocity.y;
        bouncing_rect->rgb = rand_rgb();
    } else if (bouncing_rect->rect.y <= 0) {
        bouncing_rect->velocity.y = abs(bouncing_rect->velocity.y);
        bouncing_rect->rgb = rand_rgb();
  }
    bouncing_rect->rect.x += bouncing_rect->velocity.x;
    bouncing_rect->rect.y += bouncing_rect->velocity.y;
}

void render_bouncing_rect(SDL_Renderer *prenderer, BouncingRect bouncing_rect) {
    SDL_SetRenderDrawColor(prenderer, bouncing_rect.rgb.r, bouncing_rect.rgb.g, bouncing_rect.rgb.g, bouncing_rect.rgb.a);
    SDL_RenderFillRect(prenderer, &bouncing_rect.rect);
    SDL_RenderPresent(prenderer);
}

void update_spiral_rect(BouncingRect *spiral_rect, WindowBounds window_bounds) {
    Vec2 spiral_point = get_spiral_point(spiral_rect->degree);
    spiral_rect->rect.x = spiral_point.x + (window_bounds.window_right_bound / 2);
    spiral_rect->rect.y = spiral_point.y + (window_bounds.window_bottom_bound / 2);

    if (spiral_rect->rect.x <= 0 || spiral_rect->rect.y <= 0 || spiral_rect->rect.x >= window_bounds.window_right_bound || spiral_rect->rect.y >= window_bounds.window_bottom_bound) {
        spiral_rect->degree_direction = -spiral_rect->degree_direction;
        spiral_rect->rgb = rand_rgb();
    } else if (spiral_rect->degree <= 0) {
        spiral_rect->degree_direction = fabs(spiral_rect->degree_direction);
        spiral_rect->rgb = rand_rgb();
    }
    spiral_rect->degree += spiral_rect->degree_direction;

}
void render_spiral_rect(SDL_Renderer *prenderer, BouncingRect spiral_rect) {
    SDL_SetRenderDrawColor(prenderer, spiral_rect.rgb.r, spiral_rect.rgb.g, spiral_rect.rgb.g, spiral_rect.rgb.a);
    SDL_RenderFillRect(prenderer, &spiral_rect.rect);
    SDL_RenderPresent(prenderer);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    bool quit = false;
    SDL_Event event;
    SDL_Window *pwindow = SDL_CreateWindow("RandWalk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_SOFTWARE);

    WindowBounds window_bounds = { .window_left_bound = 0, .window_top_bound = 0};
    SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

    BouncingRect bouncing_rect = {
        .rect = {.x = 0, .y = 0, .w = 5, .h = 5},
        .velocity = { .x = 10, .y = 10 },
        .rgb = rand_rgb(),
    };
    
    BouncingRect spiral_rect = {
        .rect = { .x = window_bounds.window_right_bound/2, .y = window_bounds.window_bottom_bound/2, .w = 5, .h = 5 },
        .velocity = { .x = 10, .y = 10 },
        .rgb = rand_rgb(),
        .degree = 0,
        .degree_direction = 10
    };

    while (!quit) {

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

        // update and render bouncer rect
        update_bouncing_rect(&bouncing_rect, window_bounds);
        render_bouncing_rect(prenderer, bouncing_rect);

        // render then update spiral rect
        render_spiral_rect(prenderer, spiral_rect);
        update_spiral_rect(&spiral_rect,window_bounds);
    }
}
