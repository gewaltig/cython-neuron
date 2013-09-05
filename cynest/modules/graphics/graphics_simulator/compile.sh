#!/bin/sh

cwd= pwd

cd $1
g++ main.cpp graphics_simulator.cpp socket.cpp tools.cpp window.cpp   `sdl-config --cflags --libs` -lGLU -lSDL   -o g_simulator
	
cd $cwd
