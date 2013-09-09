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

#include "tools.h"


#define EVENT_NOTHING 0
#define EVENT_QUIT 1
#define EVENT_STOP 2
#define EVENT_RESUME 3

#define ANGLE_DIFF 0.05
#define DIST_DIFF 0.5
#define PI 3.15


class Window
{
private:
	SDL_Surface *surface;
	SDL_Event event;
	Neuron* neurons;
	int* nb_neurons;
	
	// Display parameters
	int width;
	int height;
	
	double theta;
	double phi;
	double camera_dist;
	Vector3d camera_pos;

	void init_display();
	
	void init_neuron_params();
	
	void draw_connections();

public:
		void init(int width_, int height_, char* caption, Neuron* neurons_, int* nb_neurons_);
		void destroy();
		
		void resize(int width_, int height_);
		
		int handleEvents();

		void update();
		
		void draw();
};

#endif
