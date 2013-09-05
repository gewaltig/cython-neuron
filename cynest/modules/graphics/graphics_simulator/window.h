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

class Window
{
private:
	SDL_Surface *surface;

public:
		void init(int width, int height, char* caption);
};

#endif
