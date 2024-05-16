#include <math.h>
#include <stdio.h>

#include "raylib.h"

const char GAME_MAP[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1},
};

const int SCALE = 2;

typedef struct {
    int width;
    int height;
} Window;

typedef struct {
    // TODO: reduce to vec2
    float x;
    float y;
    float angle;
    int fov;
    float speed;
    float rotation_rate;
} Character;

const int PRECISION = 64;

float radians(float degree) {
    return degree * M_PI / 180;
}

Vec2 add(Vec2 a, Vec2 b) { // TODO: mutable version? scrap?
    Vec2 temp = {a.x + b.x, a.y + b.y};
    return temp;
}

Vec2 sub(Vec2 a, Vec2 b) {
    Vec2 temp = {a.x - b.x, a.y - b.y};
    return temp;
}

float dot(Vec2 a, Vec2 b) {
    return a.x*b.x + a.y*b.y;;
}

Vec2 divide(Vec2 v, float m) {
    Vec2 temp = {v.x/m, v.y/m};
    return temp;
}

float mag(Vec2 v) {
    return sqrt(dot(v, v));
}

Vec2 normalize(Vec2 v) { // does this mutate my vector?
    // should this be a ptr return?
    Vec2 temp = divide(v, mag(v));
    return temp;
}

Vec2 get_vec_step(Vec2 v) { // TODO: figure out how to not recalc per step
    return divide(normalize(v), PRECISION);
}

float get_distance(Vec2 v, Vec2 ref) { // TODO: convert to ptr passing
    float distance = mag(sub(v, ref));
    // TODO: fisheye fix (uhh fuck vectors)
    return distance;
}

void raywalk(Vec2 *ray) {
    // save step vec for this ray iteration
    Vec2 v_step = get_vec_step(*ray);

    while (GAME_MAP[(int) floor(ray->y)][(int) floor(ray->x)] == 0) {
	ray->x += v_step.x;
	ray->y += v_step.y;
    }
}

void draw_ray(Window *screen, int x, float height) {
    DrawLine(x, 0, x, (float) screen->height/2 - height, RED);
    DrawLine(x, (float) screen->height/2 - height, x, (float) screen->height/2 + height, GREEN);
    DrawLine(x, (float) screen->height/2 + height, x, screen->height, BLUE);
}

void raycast(Window *screen, Character *camera) {
    float view_angle = camera->angle - (float) camera->fov/2;

    for (int i = 0; i < screen->width; i++) {
	Vec2 ray = {camera->pos.x, camera->pos.y};
	raywalk(&ray);
	float distance = get_distance(ray, camera->pos);
	distance *= cos(radians(view_angle - camera->angle));
	float height = screen->height / (2*distance);
	draw_ray(screen, i, height);
	view_angle += (float) camera->fov / screen->width;
    }
}

int main(void) {
    Window window = {640, 480};
    Window projection = {window.width/SCALE, window.height/SCALE};
    InitWindow(window.width, window.height, "RayC");
    SetTargetFPS(60);

    Vec2 player_pos = {2, 2};
    Character player = {player_pos, 90, 60, 2, 3.5};

    while (!WindowShouldClose()) {
	BeginDrawing();

	    ClearBackground(RAYWHITE);

	    raycast(&window, &player);

	EndDrawing();
    }

    CloseWindow();

    return 0;
}
