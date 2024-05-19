#include <math.h>
#include <stdio.h>

#include "raylib.h"

#define PLAYER_FOV 60
#define PLAYER_SPEED 2
#define PLAYER_ROT_RATE 1

const int SCALE = 2;
const int PRECISION = 64;

const char GAME_MAP[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1},
    {1,0,0,1,1,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1},
};

typedef struct {
    int width;
    int height;
} Window;

typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    float x;
    float y;
    float angle;
} Vec2;

typedef struct {
    // TODO: reduce to vec2
    Vec2 pos;
    int fov;
    float speed;
    float rotation_rate;
} Character;

float radians(float degree) {
    return degree * M_PI / 180;
}

Vec2 get_vec_step(Vec2 *ray) {
    float x_step = cos(radians(ray->angle)) / PRECISION;
    float y_step = sin(radians(ray->angle)) / PRECISION;
    Vec2 step = {x_step, y_step, ray->angle};
    return step;
}

void raywalk(Vec2 *ray) {
    // save step vec for this ray iteration
    Vec2 step = get_vec_step(ray);

    while (GAME_MAP[(int) floor(ray->y)][(int) floor(ray->x)] == 0) {
	ray->x += step.x;
	ray->y += step.y;
    }
}

void draw_ray(Window *screen, int x, float height) {
    DrawLine(x, 0, x, (float) screen->height/2 - height, RED);
    DrawLine(x, (float) screen->height/2 - height, x, (float) screen->height/2 + height, GREEN);
    DrawLine(x, (float) screen->height/2 + height, x, screen->height, BLUE);
    printf("Height: %f\n", height);
    printf("Red: %f\n", (float) screen->height/2 - height);
    printf("Green: %f\n", (float) screen->height/2 + height);
    printf("Blue: %f\n", (float) screen->height/2);
}

float get_distance(Vec2 *ray, Vec2 *ref_pos) {
    float distance = sqrt((ray->x - ref_pos->x)*(ray->x - ref_pos->x) + (ray->y - ref_pos->y)*(ray->y - ref_pos->y));
    distance *= cos(radians(ray->angle - ref_pos->angle)); /* fisheye fix */
    return distance;
}

void raycast(Window *screen, Character *player) {
    float view_angle = player->pos.angle - (float) player->fov/2;

    for (int i = 0; i < screen->width; i++) {
	Vec2 ray = {player->pos.x, player->pos.y, player->pos.angle};
	raywalk(&ray);
	float distance = get_distance(&ray, &player->pos);
	float height = screen->height / (2*distance);
	draw_ray(screen, i, height);
	view_angle += (float) player->fov / screen->width;
    }
}

int main(void) {
    Window window = {320, 240};
    Window projection = {window.width/SCALE, window.height/SCALE};
    InitWindow(window.width, window.height, "RayC");
    SetTargetFPS(60);

    // TODO: make a pointer?
    Vec2 player_pos = {2, 2, 90};
    Character player = {player_pos, PLAYER_FOV, PLAYER_SPEED, PLAYER_ROT_RATE};

    while (!WindowShouldClose()) {
	if (IsKeyDown(KEY_RIGHT)) player.pos.angle += player.rotation_rate;
	else if (IsKeyDown(KEY_LEFT)) player.pos.angle -= player.rotation_rate;
	Vec2 step = get_vec_step(&player.pos);
	// TODO: modify speed according to Pythagorean theorem
	if (IsKeyDown(KEY_UP)) {
	    player.pos.x += step.x * player.speed;
	    player.pos.y += step.y * player.speed;
	} else if (IsKeyDown(KEY_UP)) {
	    player.pos.x -= step.x * player.speed;
	    player.pos.y -= step.y * player.speed;
	}

	BeginDrawing();

	    ClearBackground(RAYWHITE);

	    raycast(&window, &player);

	EndDrawing();
    }

    CloseWindow();

    return 0;
}
