# cmandelbrot
Mandelbrot explorer written in C with SDL2 and OpenGL.
Heavily inspired by [Lain69/Mandelbrot](https://github.com/Lain69/Mandelbrot-with-SDL2/)

## Prerequisites
- [SDL2](https://www.libsdl.org/)
- An OpenGL version that supports `double` (OpenGL > 4.0 or
 [ARB_gpu_shader_fp64](https://registry.khronos.org/OpenGL/extensions/ARB/ARB_gpu_shader_fp64.txt)
- [GLEW](http://glew.sourceforge.net/)

## Controls

<kbd>w</kbd> <kbd>a</kbd> <kbd>s</kbd> <kbd>d</kbd> / <kbd>left</kbd> <kbd>right</kbd> <kbd>up</kbd> <kbd>down</kbd> -
move left/right/up/down

<kbd>-</kbd> <kbd>=</kbd> - zoom in/out

<kbd>q</kbd> <kbd>e</kbd> - increase/decrease number of iterations before bailout

<kbd>r</kbd> reset position, zoom, and iterations

<kbd>lshift</kbd> - 5x control speed (coarse)
 
<kbd>lalt</kbd> - 0.01x control speed (fine)

<kbd>x</kbd> - Overlow crosshair for alignment

<kbd>p</kbd> - Save screenshot of current viewable area

<kbd>escape</kbd> - Quit

## Command Line Arguments

Pairs of key-value arguments can be passed to specify starting values.
e.g.: `./mandelbrot width 500 height 128 screenshotname "image"`

Available Options:

- `width`
- `height`
- `centerX`
- `centerY`
- `zoom`
- `screenshotname`

## Todo

- [X] Command line arguments
    - [X] Screen Size
    - [X] Screenshot Name
    - [X] Starting positions
- [ ] Save positions
