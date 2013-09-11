#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <cmath>
#include <vector>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <string>

#include "tools.h"


#define EVENT_NOTHING 0
#define EVENT_QUIT 1
#define EVENT_STOP 2
#define EVENT_RESUME 3
#define EVENT_STEP_CHANGED 4
#define EVENT_PAUSE 5
#define EVENT_RESUME 6

#define LOW_BOUND_SIM_STEP 1
#define HIGH_BOUND_SIM_STEP 10000

class Window
{
private:
	SDL_Event event;
	SDL_Surface *surface;
	Neuron* neurons;
	int* nb_neurons;
	int* simulation_step;
	double* simulation_total_time;
	char caption[200];
	bool stopped;
	
	// keyboard handling
	bool plus_pressed;
	bool minus_pressed;
	bool p_pressed;
	
	Camera camera;
	
	// Display parameters
	int width;
	int height;

	void init_display();
	
	void draw_connections();

public:
		void init(int width_, int height_, Neuron* neurons_, int* nb_neurons_, int* sim_step, double* sim_time_);
		void destroy();
		
		void resize(int width_, int height_);
		
		int handleEvents();

		void update(double time_);
		
		void draw();
};


void incrementSimulationStep(int* v);
void decrementSimulationStep(int* v);

#endif
