#!/bin/sh

cwd= pwd

cd $1
g++ -lGLU -lSDL main.cpp graphics_simulator.cpp socket.cpp tools.cpp window.cpp -o g_simulator
	
cd $cwd
