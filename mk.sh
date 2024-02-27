#!/bin/bash
flags="$(sdl2-config --libs --cflags)  -lm -lSDL2_image -lSDL2_ttf"
rm -rf *.xe

gcc -o sdl_app.xe sdl_app.c $flags -I/usr/include/SDL2 $*