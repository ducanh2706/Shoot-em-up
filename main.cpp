#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "defs.h"
#include "graphics.h"
#include "structs.h"
#include "input.h"
#include "draw.h"
#include "game.h"

void capFrameRate(long long *then, float *remainder)
{
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1)
	{
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();
}

int main(int argc, char** argv) {
    Graphics graphics;
    graphics.init();

    Game game;
    game.init(&graphics);

    Input input;
    input.init();

    long long then = SDL_GetTicks64();
    float remainder = 0;

    while (true) {
        prepareScene(graphics.renderer);
        input.get();
        game.logic(&graphics, input.keyboard);
        game.draw(&graphics);

        presentScene(graphics.renderer);
        capFrameRate(&then, &remainder);
    }
}


