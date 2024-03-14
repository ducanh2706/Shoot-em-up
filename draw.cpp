#include <SDL2/SDL.h>
#include "draw.h"

void prepareScene(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
    SDL_RenderClear(renderer);
}

void presentScene(SDL_Renderer *renderer) {
    SDL_RenderPresent(renderer);
}

void blitRect(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(renderer, texture, src, &dest);
}