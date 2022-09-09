.PHONY: clean
.PHONY: run

SRCS=$(wildcard *.c)
TARGET=mandelbrot

CC=gcc

LDFLAGS=-lSDL2main -lSDL2 -lGL -lGLEW -lGLU -lm

CFLAGS+=--std=c99
CFLAGS+=-Wall
CFLAGS+=-Wextra
CFLAGS+=-pedantic-errors

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf mandelbrot screenshot*

run: all
	./mandelbrot
