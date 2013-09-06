#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <cmath>
#include <vector>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>

#include <GL/glu.h>
#include <SDL/SDL.h>

#include "tools.h"


#define EVENT_NOTHING 0
#define EVENT_QUIT 1
#define EVENT_STOP 2
#define EVENT_RESUME 3



class Window
{
private:
	SDL_Surface *surface;
	SDL_Event event;
	vector<Neuron>* neurons;

public:
		void init(int width, int height, char* caption, vector<Neuron>* neurons_);
		void destroy();
		
		int handleEvents();
		void draw();
};

#endif
