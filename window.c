#include "window.h"

#include <stdbool.h>
#include <errno.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

/* the rendering window */
SDL_Window *window = NULL;
int width, height;

/* OpenGL context */
SDL_GLContext ctx;

/* graphics program */
GLuint programID = 0;
GLint  vertexPosAttrLoc = -1;
GLuint VBO = 0;
GLuint IBO = 0;

GLint DEFAULT_ITERATION_LIMIT = 50;
GLdouble DEFAULT_CENTER[2] = { 0, 0 };
GLdouble DEFAULT_ZOOM = 0.45;

GLint iterationLimit;
GLdouble center[2];
GLdouble zoom;
GLboolean windowCrosshair = GL_FALSE;


const char *screenShotFileBase = NULL;

bool initSDL(int w, int h, const char *vertexFile, const char *fragmentFile, const char *screenShotFile)
{
    /* SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // Use OpenGL 4.0 for double support
    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); */
    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    /* create window */
    window = SDL_CreateWindow(
            "Mandelbrot Explorer",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            w,
            h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL) {
        fprintf(stderr, "Could not create SDL window: %s\n", SDL_GetError());
        return false;
    }

    width = w;
    height = h;

    ctx = SDL_GL_CreateContext(window);
    if (ctx == NULL) {
        fprintf(stderr, "OpenGL context could not be created: %s\n", SDL_GetError());
        return false;
    }

    /* initialize GLEW */
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        fprintf(stderr, "Could not initialize GLEW: %s\n", glewGetErrorString(glewError));
    }

    /* vsync */
    if (SDL_GL_SetSwapInterval(1) < 0) {
        fprintf(stderr, "Could not set VSync: %s\n", SDL_GetError());
    }

    /* initialize OpenGL */
    if (!initGL(vertexFile, fragmentFile)) {
        fprintf(stderr, "Could not initialize OpenGL\n");
        return false;
    }

    reset();
    screenShotFileBase = screenShotFile;

    /* success */
    return true;
}

void reset()
{
    iterationLimit = DEFAULT_ITERATION_LIMIT;
    center[0] = DEFAULT_CENTER[0];
    center[1] = DEFAULT_CENTER[1];
    zoom = DEFAULT_ZOOM;
}

bool initGL(const char *vertexFile, const char *fragmentFile)
{
    /* generate program */
    programID = glCreateProgram();

    /* create vertex shader */
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    /* TODO: can I take adress */
    char *vertexShaderText = readShaderProgramFromFile(vertexFile);
    const GLchar* vertexShaderSource[] = { (GLchar*) vertexShaderText };

    /* set vertex source */
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

    /* compile vertex shader and check for success*/
    glCompileShader(vertexShader);
    /* free(vertexShaderText); /1* no longer need this *1/ */

    GLint success = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        fprintf(stderr, "Unable to compile vertex shader '%s' (%d):\n", vertexFile, vertexShader);
        printShaderLog(vertexShader);
        return false;
    }

    glAttachShader(programID, vertexShader);

    /* create fragment shader */
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    char *fragmentShaderText = readShaderProgramFromFile(fragmentFile);
    const GLchar* fragmentShaderSource[] = { (GLchar*) fragmentShaderText };

    /* set fragment source */
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);
    /* free(fragmentShaderText); /1* no longer need this *1/ */

    /* compile fragment shader and check for success */
    success = GL_FALSE;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        fprintf(stderr, "Unable to compile fragment shader '%s' (%d):\n", fragmentFile, fragmentShader);
        printShaderLog(fragmentShader);
        return false;
    }

    glAttachShader(programID, fragmentShader);

    glLinkProgram(programID);

    success = GL_TRUE;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        fprintf(stderr, "Error linking shader program %d\n", programID);
        printProgramLog(programID);
        return false;
    }

    /* get vertex attribute location */
    vertexPosAttrLoc = glGetAttribLocation(programID, "position");
    if (vertexPosAttrLoc == -1) {
        fprintf(stderr, "position is not a valid glsl program variable\n");
        return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* VBO data, full screen */
    GLfloat vertexData[] = {
       -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
       -1.0f,  1.0f,
    };

    /* IBO data */
    GLuint indexData[] = { 0, 1, 2, 3 };

    /* create VBO */
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 2*4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

    /* create IBO */
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

    /* success */
    return true;
}

void handleInput(const uint8_t *keyboard)
{
    double turbo = 1 + 4*keyboard[SDL_SCANCODE_LSHIFT];

    if (keyboard[SDL_SCANCODE_LALT]) {
        /* ~foine~ control */
        turbo *= 0.01;
    }

    int left =  keyboard[SDL_SCANCODE_LEFT]  | keyboard[SDL_SCANCODE_A];
    int right = keyboard[SDL_SCANCODE_RIGHT] | keyboard[SDL_SCANCODE_D];
    int up =    keyboard[SDL_SCANCODE_UP]    | keyboard[SDL_SCANCODE_W];
    int down =  keyboard[SDL_SCANCODE_DOWN]  | keyboard[SDL_SCANCODE_S];

    center[1] += turbo * (up - down) * 0.1 / zoom;
    center[0] += turbo * (right - left) * 0.1 / zoom;

    int zoomDirection = keyboard[SDL_SCANCODE_EQUALS] - keyboard[SDL_SCANCODE_MINUS];

    zoom = fmax(0.2, zoom * pow(2, turbo * zoomDirection * 0.5));

    int iterIncrease = keyboard[SDL_SCANCODE_E];
    int iterDecrease = keyboard[SDL_SCANCODE_Q];
    iterationLimit = (GLint)(fmax(0, fmin(1000, iterationLimit+turbo*5*(iterIncrease-iterDecrease))));

    if (keyboard[SDL_SCANCODE_P]) {
        screenshot(screenShotFileBase);
    }

    if (keyboard[SDL_SCANCODE_R]) {
        reset();
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(programID);

    GLint resUniform = glGetUniformLocation(programID, "u_resolution");
    glUniform2i(resUniform, width, height);

    GLint iterUniform = glGetUniformLocation(programID, "u_iterationLimit");
    glUniform1i(iterUniform, iterationLimit);

    GLint centerUniform = glGetUniformLocation(programID, "u_center");
    glUniform2d(centerUniform, center[0], center[1]);

    GLint zoomUniform = glGetUniformLocation(programID, "u_zoom");
    glUniform1d(zoomUniform, zoom);

    GLint crossUniform = glGetUniformLocation(programID, "u_crosshair");
    glUniform1i(crossUniform, windowCrosshair);

    glEnableVertexAttribArray(vertexPosAttrLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(vertexPosAttrLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    /* set index data & render */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(vertexPosAttrLoc);
    glUseProgram(0);

    /* update screen */
    SDL_GL_SwapWindow(window);

    printf("\rCenter: (%+1.08f, %+1.08f), Zoom: %10.4f, Iterations: %5d", center[0], center[1], zoom, iterationLimit);
    fflush(stdout);
}

void cleanup()
{
    glDeleteProgram(programID);

    /* destroy window */
    SDL_DestroyWindow(window);
    window = NULL;

    SDL_Quit();
}

void printProgramLog(GLuint id)
{
    if (glIsProgram(id)) {
        int infoLogLength = 0;
        int maxLength = 0;

        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        char *log = malloc(maxLength);
        if (!log) {
            fprintf(stderr, "Could not allocation memory %s\n", strerror(errno));
            exit(1);
        }

        glGetProgramInfoLog(id, maxLength, &infoLogLength, log);
        if (infoLogLength > 0) {
            fprintf(stderr, "%s\n", log);
        }

        free(log);
        exit(1);
    } else {
        fprintf(stderr, "Name %d is not a program\n", id);
    }
}

void printShaderLog(GLuint id)
{
    if (glIsShader(id)) {
        /* shader log length */
        int infoLogLength = 0;
        int maxLength = 0;

        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        char *log = malloc(maxLength);
        if (!log) {
            fprintf(stderr, "Could not allocation memory %s\n", strerror(errno));
            exit(1);
        }

        glGetShaderInfoLog(id, maxLength, &infoLogLength, log);
        if (infoLogLength > 0) {
            fprintf(stderr, "%s\n", log);
        }

        free(log);
        exit(1);
    } else {
        fprintf(stderr, "Name %d is not a shader\n", id);
    }
}

char* readShaderProgramFromFile(const char *file)
{
    FILE *fptr = fopen(file, "r");
    if (!fptr) {
        fprintf(stderr, "Could not open '%s' for reading: %s\n", file, strerror(errno));
        return NULL;
    }

    size_t totalRead = 0;
    size_t capacity = 4096;

    char *buffer = malloc(sizeof(*buffer) * capacity);
    if (!buffer) {
        fprintf(stderr, "Could not allocate buffer: %s\n", strerror(errno));
        return NULL;
    }

    for (;;) { /* read file data */
        /* resize if necessary */
        if (totalRead >= capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (!buffer) {
                fprintf(stderr, "Could not allocate buffer: %s\n", strerror(errno));
                return NULL;
            }
        }

        size_t n = fread(buffer, 1, capacity-totalRead-1, fptr);
        if (n == 0) { /* no more to read */
            break;
        }

        totalRead += n;
    }

    /* guaranteed room for null */
    buffer[totalRead] = '\0';

    return buffer;
}

void screenshot(const char *basefile) {
    size_t len = (basefile && *basefile) ? strlen(basefile) : 0;
    char *filename = malloc(len + 6 + 4 + 1);
    if (!filename) {
        fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
        exit(1);
    }

    strncpy(filename, basefile, len);
    for (size_t i = len; i < len+6; i++) {
        char c = (char) rand();
        while (!isalnum(c)) {
            c = (char) rand();
        }
        filename[i] = c;
    }

    strncpy(filename+len+6, ".bmp", 5);

    unsigned char *pixels = malloc(width * height * 4);
    if (!pixels) {
        fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
        exit(1);
    }

    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);

    int flag = 1;
    uint32_t r, g, b, a; /* masks */
    r = g = b = a = 0;
    if (*((char*) &flag)) {
        /* little endian */
        r = 0x000000FF;
        g = 0x0000FF00;
        b = 0x00FF0000;
        a = 0xFF000000;
    }

    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels, width, height, 8*4, width*4, r, g, b, a);
    int err = SDL_SaveBMP(surf, filename);
    if (err != 0) {
        fprintf(stderr, "Could not save '%s': %s'", filename, SDL_GetError());
    } else {
        printf("\nSaved screenshot '%s'\n", filename);
        fflush(stdout);
    }

    SDL_FreeSurface(surf);

    free(pixels);
    free(filename);
}
