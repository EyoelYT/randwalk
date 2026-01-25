#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PI 3.14159265358979323846
#define MAX_RECTS 100
#define ROTATION_VELOCITY 1

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
    Vec2 center;
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

typedef struct {
    void (*update_fn_ptrs[3])(BouncingRect *bouncing_rect, const WindowBounds window_bounds);
    int update_fn_idx;
    int num_update_fns;
} Updaters;

RGB rand_rgb() {
    RGB color;
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    color.a = 255;
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

void render_rand_walk_rect(SDL_Renderer *prenderer, BouncingRect bouncing_rect) {
    SDL_SetRenderDrawColor(prenderer, bouncing_rect.rgb.r, bouncing_rect.rgb.g, bouncing_rect.rgb.b, bouncing_rect.rgb.a);
    SDL_RenderFillRect(prenderer, &bouncing_rect.rect);
}

void update_spiral_rect(BouncingRect *spiral_rect, const WindowBounds window_bounds) {
    Vec2 next_spiral_point = get_spiral_point(spiral_rect->degree);

    spiral_rect->rect.x = spiral_rect->center.x + next_spiral_point.x;
    spiral_rect->rect.y = spiral_rect->center.y + next_spiral_point.y;

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
        // curr is (s,0) or (-s,0) -> choose among (0, s), (0,−s)
        choices[0] = (Vec2){ 0, s };
        choices[1] = (Vec2){ 0, -s };
    } else {
        // curr is (0,s) or (0,−s) -> choose among ( s,0), (−s,0)
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

void update_rand_walk_rect(BouncingRect *rand_walk_rect, const WindowBounds window_bounds) {
    // update direction when collision
    if (rand_walk_rect->rect.x <= window_bounds.window_left_bound) {
        rand_walk_rect->rect.x = window_bounds.window_left_bound + 1;
        rand_walk_rect->velocity.x = -rand_walk_rect->velocity.x;
        rand_walk_rect->rgb = rand_rgb();
        rand_walk_rect->step_tracker = 0;
    }
    if (rand_walk_rect->rect.x + rand_walk_rect->rect.w >= window_bounds.window_right_bound) {
        rand_walk_rect->rect.x = window_bounds.window_right_bound - rand_walk_rect->rect.w - 1;
        rand_walk_rect->velocity.x = -rand_walk_rect->velocity.x;
        rand_walk_rect->rgb = rand_rgb();
        rand_walk_rect->step_tracker = 0;
    }
    if (rand_walk_rect->rect.y <= window_bounds.window_top_bound) {
        rand_walk_rect->rect.y = window_bounds.window_top_bound + 1;
        rand_walk_rect->velocity.y = -rand_walk_rect->velocity.y;
        rand_walk_rect->rgb = rand_rgb();
        rand_walk_rect->step_tracker = 0;
    }
    if (rand_walk_rect->rect.y + rand_walk_rect->rect.h >= window_bounds.window_bottom_bound) {
        rand_walk_rect->rect.y = window_bounds.window_bottom_bound - rand_walk_rect->rect.h - 1;
        rand_walk_rect->velocity.y = -rand_walk_rect->velocity.y;
        rand_walk_rect->rgb = rand_rgb();
        rand_walk_rect->step_tracker = 0;
    }
    // update step tracker

    // int num_steps = rand_num(5, 100, 0);
    int num_steps = 5;
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

void clear_screen(SDL_Renderer *prenderer) {
    SDL_SetRenderDrawColor(prenderer, 0, 0, 0, 255);
    SDL_RenderClear(prenderer);
}

Vec2 increase_magnitude(Vec2 v) {
    if (v.x > 0)  ++v.x;
    else if (v.x < 0)  --v.x;
    if (v.y > 0)  ++v.y;
    else if (v.y < 0)  --v.y;
    return v;
}

Vec2 decrease_magnitude(Vec2 v) {
    if (v.x > 1)  --v.x;
    else if (v.x < -1)  ++v.x;
    if (v.y > 1)  --v.y;
    else if (v.y < -1)  ++v.y;
    return v;
}

int find_next_number(int prev, int min_include, int max_exclude) {
    if (prev < max_exclude) return ++prev;
    return min_include;
}

void handle_keypresses(SDL_Keycode keycode, SDL_Renderer *prenderer, bool *clear_frame, int *rand_walk_rect_count, Updaters *updaters, BouncingRect *rand_walk_rects, WindowBounds window_bounds, int rect_velocity, int pixel_size) {
    switch (keycode) {
        case SDLK_0: {
            clear_screen(prenderer);
            break;
        }
        case SDLK_c: {
            *clear_frame = !*clear_frame;
            break;
        }
        case SDLK_p: {
            for (int i = 0; i < *rand_walk_rect_count; i++) {
                rand_walk_rects[i].rect.w++;
                rand_walk_rects[i].rect.h++;
            }
            break;
        }
        case SDLK_v: {
            for (int i = 0; i < *rand_walk_rect_count; i++) {
                rand_walk_rects[i].velocity = increase_magnitude(rand_walk_rects[i].velocity);
            }
            break;
        }
        case SDLK_i: {
            if (*rand_walk_rect_count > 0) {
                *rand_walk_rect_count -= 1;
            }
            break;
        }
        case SDLK_d: {
            rand_walk_rects[*rand_walk_rect_count] = (BouncingRect) {
                .rect = { .x = rand_num(1, window_bounds.window_right_bound, 0),
                          .y = rand_num(1, window_bounds.window_bottom_bound, 0),
                          .w = pixel_size,
                          .h = pixel_size },
                .center = {
                    .x = rand_num(1, window_bounds.window_right_bound, 0),
                    .y = rand_num(1, window_bounds.window_bottom_bound, 0),
                },
                .velocity = { .x = rect_velocity,
                              .y = 0 },
                .rgb = rand_rgb(),
                .degree = 0,
                .degree_direction = ROTATION_VELOCITY
            };
            if (*rand_walk_rect_count > 0) {
                    *rand_walk_rect_count -= 1;
            }
            break;
        }
        case SDLK_u: {
            updaters->update_fn_idx = rand_num(0, updaters->num_update_fns - 1, 0);
            // updaters->update_fn_idx = find_next_number(updaters->update_fn_idx, 0, updaters->num_update_fns - 1);
            for (int i = 0; i < *rand_walk_rect_count; i++) {
                rand_walk_rects[i].center.x = rand_walk_rects[i].rect.x;
                rand_walk_rects[i].center.y = rand_walk_rects[i].rect.y;
                rand_walk_rects[i].degree = 0;
                rand_walk_rects[i].degree_direction = ROTATION_VELOCITY;
            }
            break;
        }
        case SDLK_SPACE: {
            if (*rand_walk_rect_count > 0) {
                    *rand_walk_rect_count = 0;
            }
            else {
                *rand_walk_rect_count = MAX_RECTS;
            }
            break;
        }
    }
}

void handle_shift_keypresses(SDL_Keycode keycode, int *rand_walk_rect_count, BouncingRect *rand_walk_rects) {
    switch(keycode) {
        case SDLK_p: {
            for (int i = 0; i < *rand_walk_rect_count; i++) {
                if (rand_walk_rects[i].rect.w > 0) {
                    rand_walk_rects[i].rect.w--;
                }
                if (rand_walk_rects[i].rect.h > 0) {
                    rand_walk_rects[i].rect.h--;
                }
            }
            break;
        }
        case SDLK_v: {
            for (int i = 0; i < *rand_walk_rect_count; i++) {
                rand_walk_rects[i].velocity = decrease_magnitude(rand_walk_rects[i].velocity);
            }
            break;
        }
        case SDLK_i: {
            if (*rand_walk_rect_count < MAX_RECTS) {
                    *rand_walk_rect_count += 1;
            }
            break;
        }
        case SDLK_d: {
            if (*rand_walk_rect_count < MAX_RECTS) {
                    *rand_walk_rect_count += 1;
            }
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    bool clear_frame = false;

    int screen_width = 800;
    int screen_height = 600;

    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
        screen_width = dm.w;
        screen_height = dm.h;
    }

    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    bool quit = false;
    SDL_Event event;
    SDL_Window *pwindow = SDL_CreateWindow("RandWalk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer *prenderer = SDL_CreateRenderer(pwindow, -1, SDL_RENDERER_SOFTWARE);

    int rand_walk_rect_count = MAX_RECTS;
    int pixel_size = 1;
    int rect_velocity = 1;
    int clear_frame_counter = 0;

    WindowBounds window_bounds = { .window_left_bound = 0, .window_top_bound = 0};
    SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

    BouncingRect rand_walk_rects[rand_walk_rect_count];
    for (int i = 0; i < rand_walk_rect_count; i++) {
        rand_walk_rects[i] = (BouncingRect) {
            .rect = { .x = window_bounds.window_right_bound/2, .y = window_bounds.window_bottom_bound/2, .w = pixel_size, .h = pixel_size },
            .velocity = { .x = rect_velocity, .y = 0 },
            .rgb = rand_rgb(),
            .degree = 0,
            .degree_direction = ROTATION_VELOCITY
        };
    }

    Updaters updaters = {
        .update_fn_ptrs = {
            update_rand_walk_rect,
            update_spiral_rect,
            update_bouncing_rect,
        },
        .update_fn_idx = 0,
        .num_update_fns = sizeof(((Updaters *)0)->update_fn_ptrs) / sizeof(((Updaters *)0)->update_fn_ptrs[0])
    };

    while (!quit) {

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.key.keysym.mod & KMOD_SHIFT) {
                handle_shift_keypresses(event.key.keysym.sym, &rand_walk_rect_count, rand_walk_rects);
            } else if (event.type == SDL_KEYDOWN) {
                handle_keypresses(event.key.keysym.sym, prenderer, &clear_frame, &rand_walk_rect_count, &updaters, rand_walk_rects, window_bounds, rect_velocity, pixel_size);
            }
        }
        if (clear_frame) {
            if (clear_frame_counter >= 25) {
                clear_screen(prenderer);
                clear_frame_counter = 0;
            } else {
                clear_frame_counter++;
            }
        }
        SDL_GetWindowSize(pwindow, &window_bounds.window_right_bound, &window_bounds.window_bottom_bound);

        for (int i = 0; i < rand_walk_rect_count; i++) {
            // update_rand_walk_rect(&rand_walk_rects[i], window_bounds);
            updaters.update_fn_ptrs[updaters.update_fn_idx](&rand_walk_rects[i], window_bounds);
            render_rand_walk_rect(prenderer, rand_walk_rects[i]);
        }

        SDL_RenderPresent(prenderer);
    }
}
