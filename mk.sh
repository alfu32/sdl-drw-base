#!/bin/bash
flags="$(sdl2-config --libs --cflags)  -lm -lSDL2_image -lSDL2_ttf"
rm -rf *.xe

g++ -o sfml_app.xe sfml_app.cpp -lsfml-graphics -lsfml-window -lsfml-system
gcc -o sdl_app.xe sdl_app.c $flags -I/usr/include/SDL2