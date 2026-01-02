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
    int step_tracker;
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
    color.a = 0;
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
    if (bouncing_rect->rect.x + bouncing_rect->rect.w >= window_bounds.window_right_bound) {
        bouncing_rect->velocity.x = -bouncing_rect->velocity.x;
        bouncing_rect->rgb = rand_rgb();
    } else if (bouncing_rect->rect.x <= 0) {
        bouncing_rect->velocity.x = abs(bouncing_rect->velocity.x);
        bouncing_rect->rgb = rand_rgb();
    }
    if (bouncing_rect->rect.y + bouncing_rect->rect.h >= window_bounds.window_bottom_bound) {
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

    if (spiral_rect->rect.x <= 0 ||
        spiral_rect->rect.y <= 0 ||
        spiral_rect->rect.x + spiral_rect->rect.w >= window_bounds.window_right_bound ||
        spiral_rect->rect.y + spiral_rect->rect.h >= window_bounds.window_bottom_bound) {
        spiral_rect->degree_direction = -spiral_rect->degree_direction;
        spiral_rect->rgb = rand_rgb();
    } else if (spiral_rect->degree <= 0) {
        spiral_rect->degree_direction = fabs(spiral_rect->degree_direction);
        spiral_rect->rgb = rand_rgb();
    }
    spiral_rect->degree += spiral_rect->degree_direction;
}

Vec2 rand_direction_two_choices(Vec2 curr) {
    Vec2 choices[2];
    int s = curr.x != 0 ? curr.x : curr.y;

    if (curr.x != 0) {
        /* curr is (s,0) or (-s,0) -> choose among (0, s), (0,−s) */
        choices[0] = (Vec2){ 0, s };
        choices[1] = (Vec2){ 0, -s };
    } else {
        /* curr is (0,s) or (0,−s) -> choose among ( s,0), (−s,0) */
        choices[0] = (Vec2){ s, 0 };
        choices[1] = (Vec2){ -s, 0 };
    }

    return choices[rand() % 2];
}

int rand_num(int min, int max, int constant) {
    if (constant != 0) {
        return constant;
    }
    return (rand() % (max - min + 1)) + min;
}

void update_rand_walk_rect(BouncingRect *rand_walk_rect, WindowBounds window_bounds, int num_steps) {
    // update direction when collision
    if (rand_walk_rect->rect.x < window_bounds.window_left_bound ||
        rand_walk_rect->rect.x + rand_walk_rect->rect.w >= window_bounds.window_right_bound) {
        rand_walk_rect->velocity.x = -rand_walk_rect->velocity.x;
        rand_walk_rect->rgb = rand_rgb();
    }
    if (rand_walk_rect->rect.y < window_bounds.window_top_bound ||
        rand_walk_rect->rect.y + rand_walk_rect->rect.h >= window_bounds.window_bottom_bound) {
        rand_walk_rect->velocity.y = -rand_walk_rect->velocity.y;
        rand_walk_rect->rgb = rand_rgb();
    }
    // update step tracker
    if (rand_walk_rect->step_tracker >= num_steps) {
        rand_walk_rect->step_tracker = 0;
        rand_walk_rect->velocity = rand_direction_two_choices(rand_walk_rect->velocity);
    } else {
        rand_walk_rect->step_tracker++;
    }
    // update position
    rand_walk_rect->rect.x += rand_walk_rect->velocity.x;
    rand_walk_rect->rect.y += rand_walk_rect->velocity.y;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    bool quit = false;
    SDL_Event event;
    SDL_Window *pwindow = SDL_CreateWindow("RandWalk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_SOFTWARE);

    const int rand_walk_rect_count = 5;

    WindowBounds window_bounds = { .window_left_bound = 0, .window_top_bound = 0};
    SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

    BouncingRect bouncing_rect = {
        .rect = {.x = 0, .y = 0, .w = 5, .h = 5},
        .velocity = { .x = 5, .y = 5 },
        .rgb = rand_rgb(),
    };
    
    BouncingRect spiral_rect = {
        .rect = { .x = window_bounds.window_right_bound/2, .y = window_bounds.window_bottom_bound/2, .w = 5, .h = 5 },
        .velocity = { .x = 10, .y = 10 },
        .rgb = rand_rgb(),
        .degree = 0,
        .degree_direction = 10
    };

    BouncingRect rand_walk_rect = {
        .rect = { .x = window_bounds.window_right_bound/2, .y = window_bounds.window_bottom_bound/2, .w = 5, .h = 5 },
        .velocity = { .x = 0, .y = 5 },
        .rgb = rand_rgb(),
    };
    BouncingRect rand_walk_rects[rand_walk_rect_count];
    for (int i = 0; i < rand_walk_rect_count; i++) {
        rand_walk_rects[i] = (BouncingRect) {
            .rect = { .x = window_bounds.window_right_bound/2, .y = window_bounds.window_bottom_bound/2, .w = 5, .h = 5 },
            .velocity = { .x = 5, .y = 0 },
            .rgb = rand_rgb(),
        };
    }

    while (!quit) {

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

        update_bouncing_rect(&bouncing_rect, window_bounds);
        update_spiral_rect(&spiral_rect, window_bounds);
        update_rand_walk_rect(&rand_walk_rect, window_bounds, rand_num(50, 100, 50));

        render_bouncing_rect(prenderer, spiral_rect);
        render_bouncing_rect(prenderer, bouncing_rect);
        render_bouncing_rect(prenderer, rand_walk_rect);
        for (int i = 0; i < rand_walk_rect_count; i++) {
            update_rand_walk_rect(&rand_walk_rects[i], window_bounds, rand_num(50, 100, 50));
            render_bouncing_rect(prenderer, rand_walk_rects[i]);
        }

    }
}
