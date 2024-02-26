#!/bin/bash


g++ -o sfml_app.xe sfml_app.cpp -lsfml-graphics -lsfml-window -lsfml-system
gcc -o sdl_app.xe sdl_app.c `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm -lSDL2_ttf -I/usr/include/SDL2