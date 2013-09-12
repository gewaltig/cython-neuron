#ifndef INCLUDES_H
#define INCLUDES_H

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>


#include <string>
#include <vector>
#include <iostream>

#include <pthread.h>

#include <GL/glu.h>
#include <SDL/SDL.h>
#include "SDL_ttf.h"

// OS functions
#if defined (_WIN32)
	#include <dos.h>
#else
	#include <unistd.h>
#endif

#endif
