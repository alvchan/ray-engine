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
    Vector2 *pos;
    int fov;
    float speed;
    float rotation_rate;
} Character;

float radians(float degrees) {
    return degrees * (M_PI / 180);
}

Vector2 add_vec(Vector2 *u, Vector2 *v) {
    return (Vector2){u->x + v->x, u->y + v->y};
}

void add_vec_mut(Vector2 *u, Vector2 *v) {
    u->x += v->x;
    u->y += v->y;
}

void sub_vec_mut(Vector2 *u, Vector2 *v) {
    u->x -= v->x;
    u->y -= v->y;
}

Vector2 mult_vec(Vector2 *u, float a) {
    return (Vector2){u->x * a, u->y * a};
}

void mult_vec_mut(Vector2 *u, float a) {
    u->x *= a;
    u->y *= a;
}

Vector2 divide_vec_scalar(Vector2 *u, float a) {
    return (Vector2){u->x/a, u->y/a};
}

void rotate_vec(Vector2 *u, float theta) {
    theta = radians(theta);
    u->x = u->x * cos(theta) - u->y * sin(theta);
    u->y = u->x * sin(theta) - u->y * cos(theta);
}

float get_magnitude(Vector2 *u) {
    return sqrt(u->x*u->x + u->y*u->y);
}

Vector2 normalize(Vector2 *u) {
    float magnitude = get_magnitude(u);
    return (Vector2){u->x/magnitude, u->y/magnitude};
}

Vector2 get_vec_step(Vector2 *u) {
    // TODO: swap for dda system
    // TODO: swap // for /* */ -> correct c comments
    return divide_vec_scalar(u, PRECISION);
}

float get_distance(Vector2 *ray, Vector2 *ref) {
    float distance = get_magnitude(&(Vector2){ray->x - ref->x, ray->y - ref->y});
    //distance *= cos(radians(ray->angle - ref_pos->angle)); /* fisheye fix */
    return distance;
}

void raywalk(Vector2 *ray) {
    Vector2 step = get_vec_step(ray);

    while (GAME_MAP[(int) floor(ray->y)][(int) floor(ray->x)] == 0) {
	add_vec_mut(ray, &step);
    }
}

void draw_ray(Window *screen, int x, float height, Character *player, Vector2 *ray) {
    DrawLine(x, 0, x, (float) screen->height/2 - height, RED);
    DrawLine(x, (float) screen->height/2 - height, x, (float) screen->height/2 + height, GREEN);
    DrawLine(x, (float) screen->height/2 + height, x, screen->height, BLUE);
    DrawCircle(player->pos->x * 10, player->pos->y * 10, 5, PURPLE);
    DrawCircle(ray->x, ray->y, 1, GRAY);
}

void raycast(Window *screen, Character *player) {
    float view_angle = player->pos.angle - (float) player->fov/2;

    for (int i = 0; i < screen->width; i++) {
	Vector2 ray = {player->pos.x, player->pos.y, view_angle};
	raywalk(&ray);
	float distance = get_distance(&ray, &player->pos);
	float height = screen->height / (2*distance);
	draw_ray(screen, i, height, player, &ray);
	view_angle += (float) player->fov / screen->width;
    }
}

int main(void) {
    Window window = {320, 240};
    Window projection = {window.width/SCALE, window.height/SCALE};
    InitWindow(window.width, window.height, "RayC");
    SetTargetFPS(60);

    Vector2 *player_pos = &(Vector2){2, 2};
    Character player = {player_pos, PLAYER_FOV, PLAYER_SPEED, PLAYER_ROT_RATE};

    while (!WindowShouldClose()) {
	if (IsKeyDown(KEY_RIGHT)) rotate_vec(player.pos, player.rotation_rate);
	else if (IsKeyDown(KEY_LEFT)) rotate_vec(player.pos, -player.rotation_rate);
	Vector2 step = get_vec_step(player.pos);
	mult_vec_mut(&step, player.speed);
	// TODO: modify speed according to Pythagorean theorem
	if (IsKeyDown(KEY_UP)) add_vec_mut(player.pos, &step);
	else if (IsKeyDown(KEY_UP)) sub_vec_mut(player.pos, &step);

	BeginDrawing();

	    ClearBackground(RAYWHITE);

	    raycast(&window, &player);

	EndDrawing();
    }

    CloseWindow();

    return 0;
}
