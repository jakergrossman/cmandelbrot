#ifndef MANDELBROT_WINDOW_H
#define MANDELBROT_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include <GL/glew.h>

extern GLboolean windowCrosshair;

/* starts up SDL, creates window, and initializes OpenGL */
bool initSDL(int w, int h, const char *vertexFile, const char *fragmentFile, const char *screenShotFile);

/* initialize rendering program and clear color */
bool initGL(const char *vertexFile, const char *fragmentFile);

/* input handler */
void handleInput(const uint8_t *keyboard);

/* update times */
void update();

/* reset mandelbrot parameters */
void reset();

/* render quad to the screen */
void render();

/* cleanup */
void cleanup();

/* shader loading utility  programs */
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

/* read shader from file on disk */
char* readShaderProgramFromFile(const char *file);

void screenshot(const char *basefile);


#endif

