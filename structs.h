#ifndef _PLAYER__H
#define _PLAYER__H

#include <SDL2/SDL.h>
#include <math.h>

struct Entity {
    float x;
    float y;
    int w;
    int h;
    float dx;
    float dy;
    int health;
    int reload;
    int side;
    SDL_Texture *texture;

    bool collision(Entity *other){
        return (std::max(x, other->x) < std::min(x + w, other->x + other->w)) && (std::max(y, other->y) < std::min(y + h, other->y + other->h));
    }
};

struct Explosion {
    float x;
    float y;
    float dx;
    float dy;
    int r, g, b, a;

    Explosion(float _x = 0, float _y = 0, float _dx = 0, float _dy = 0, int _r = 0, int _g = 0, int _b = 0, int _a = 0):
        x(_x), y(_y), dx(_dx), dy(_dy), r(_r), g(_g), b(_b), a(_a) {}
};

struct Debris {
    float x;
    float y;
    float dx;
    float dy;
    SDL_Rect rect;
    SDL_Texture *texture;
    int life;
};

struct Star {
    int x;
    int y;
    int speed;
};

#endif
