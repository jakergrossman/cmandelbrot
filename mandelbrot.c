#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "window.h"

int width = 1280;
int height = 728;

const char * const VERTEX_SHADER = "identity.vert";
const char * const FRAGMENT_SHADER = "mandelbrot.frag";

const char * baseScreenShotName = "screenshot";

struct windowState optionState = { 0 };

#define O_CENTERX    (1 << 1)
#define O_CENTERY    (1 << 2)
#define O_ITERATIONS (1 << 3)
#define O_ZOOM       (1 << 4)
uint8_t optionMask = 0;

void usage(const char *exe)
{
    fprintf(stderr, "Usage: %s\n [<optionName> <value>]...\n", exe);
    fprintf(stderr, "Available Options: { width, height, centerX, centerY, iterations, zoom, screenshotname }\n");
}

void parseCommandLine(int argc, char **argv)
{
    if ((argc-1) % 2 != 0) {
        fprintf(stderr, "Odd number of arguments passed to %s\n", argv[0]);
        usage(argv[0]);
        exit(1);
    }

    int intValue = 0;
    double doubleValue = 0;
    char *save = NULL;
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "width") == 0) {
            intValue = strtol(argv[i+1], &save, 10);
            if (save && *save) {
                fprintf(stderr, "Invalid integer '%s'\n", argv[i+1]);
                exit(1);
            }
            width = intValue;
        } else if (strcmp(argv[i], "height") == 0) {
            intValue = strtol(argv[i+1], &save, 10);
            if (save && *save) {
                fprintf(stderr, "Invalid integer '%s'\n", argv[i+1]);
                exit(1);
            }
            height = intValue;
        } else if (strcmp(argv[i], "centerX") == 0) {
            doubleValue = strtod(argv[i+1], &save);
            if (save && *save) {
                fprintf(stderr, "Invalid double '%s'\n", argv[i+1]);
                exit(1);
            }
            optionState.center[0] = doubleValue;
            optionMask |= O_CENTERX;
        } else if (strcmp(argv[i], "centerY") == 0) {
            doubleValue = strtod(argv[i+1], &save);
            if (save && *save) {
                fprintf(stderr, "Invalid double '%s'\n", argv[i+1]);
                exit(1);
            }
            optionState.center[1] = doubleValue;
            optionMask |= O_CENTERY;
        } else if (strcmp(argv[i], "iterations") == 0) {
            intValue = strtol(argv[i+1], &save, 10);
            if (save && *save) {
                fprintf(stderr, "Invalid integer '%s'\n", argv[i+1]);
                exit(1);
            }
            optionState.iterationLimit = intValue;
            optionMask |= O_ITERATIONS;
        } else if (strcmp(argv[i], "zoom") == 0) {
            doubleValue = strtod(argv[i+1], &save);
            if (save && *save) {
                fprintf(stderr, "Invalid double '%s'\n", argv[i]);
                exit(1);
            }
            optionState.zoom = doubleValue;
            optionMask |= O_ZOOM;
        } else if (strcmp(argv[i], "screenshotname") == 0) {
            baseScreenShotName = argv[i+1];
        } else {
            fprintf(stderr, "Unknown option '%s'\n", argv[i]);
            usage(argv[0]);
            exit(1);
        }
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    parseCommandLine(argc, argv);

    /* start up SDL and create window */
    if (!initSDL(width, height, VERTEX_SHADER, FRAGMENT_SHADER, baseScreenShotName)) {
        fprintf(stderr, "Failed to initialize SDL\n");
        exit(1);
    }

    if (optionMask & O_CENTERX) {
        state.center[0] = optionState.center[0];
    }

    if (optionMask & O_CENTERY) {
        state.center[1] = optionState.center[1];
    }

    if (optionMask & O_ZOOM) {
        state.zoom = optionState.zoom;
    }

    if (optionMask & O_ITERATIONS) {
        state.iterationLimit = optionState.iterationLimit;
    }

    /* main loop flag */
    bool quit = false;

    /* event handler */
    SDL_Event e;

    while (!quit) { /* until quit */
        state.windowCrosshair = state.keyboard[SDL_SCANCODE_X];

        if (state.keyboard[SDL_SCANCODE_ESCAPE]) {
            quit = true;
        }

        /* handle events in event queue */
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {                   /* user request quit */
                quit = true;
            }
        }
        handleInput();

        update();
        render();
    }

    puts("");
    fflush(stdout);

    /* clean up after ourselves */
    cleanup();

    exit(0);
}
