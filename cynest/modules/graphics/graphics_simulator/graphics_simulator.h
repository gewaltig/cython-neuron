#ifndef GRAPHICS_SIMULATOR_H
#define GRAPHICS_SIMULATOR_H

#include <iostream>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>  
#include <string>
#include <vector>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include <pthread.h>

#include "socket.h"
#include "tools.h"
#include "window.h"

#if defined (_WIN32)
	#include <dos.h>
#else
	#include <unistd.h>
#endif


#define RANDOM_POS_LOW -20.0
#define RANDOM_POS_HIGH 20.0

class GraphicsSimulator
{
private:
	void init_connection(int port_send, int port_receive);
	void init_window(int window_width, int window_height, char* caption);
	
	void receive_positions();
	void receive_connections();

	Window window;

	double generateRandomNumber(double low, double high);

public:
	Socket listener;
	Socket sender;
	vector<Neuron> neurons_;
	Neuron* neurons;
	int nb_neurons;
	
	void initialize(int port_send, int port_receive, int window_width, int window_height, char* caption);
	
	void start();
	
	void finalize();
};

void* detect_spikes(void* simulator);

#endif
