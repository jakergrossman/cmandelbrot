#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "window.h"

const int WIDTH = 1280;
const int HEIGHT = 728;

const char * const VERTEX_SHADER = "identity.vert";
const char * const FRAGMENT_SHADER = "mandelbrot.frag";

const char * baseScreenShotName = "screenshot";

int main(int argc, char **argv)
{
    srand(time(NULL));
    (void) argc;
    (void) argv;
    /* start up SDL and create window */
    if (!initSDL(WIDTH, HEIGHT, VERTEX_SHADER, FRAGMENT_SHADER, baseScreenShotName)) {
        fprintf(stderr, "Failed to initialize SDL\n");
        exit(1);
    }

    /* main loop flag */
    bool quit = false;

    /* event handler */
    SDL_Event e;

    const uint8_t *keyboard = SDL_GetKeyboardState(NULL);

    while (!quit) { /* until quit */
        windowCrosshair = keyboard[SDL_SCANCODE_X];

        if (keyboard[SDL_SCANCODE_ESCAPE]) {
            quit = true;
        }

        /* handle events in event queue */
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {                   /* user request quit */
                quit = true;
            }
        }
        handleInput(keyboard);

        update();
        render();
    }

    puts("");
    fflush(stdout);

    /* clean up after ourselves */
    cleanup();

    exit(0);
}
