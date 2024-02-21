#!/bin/bash


g++ -o sfml_app.xe sfml_app.cpp -lsfml-graphics -lsfml-window -lsfml-system
gcc -o sdl_app.xe sdl_app.c -lm -lSDL2 -lSDL2_ttf -I/usr/include/SDL2