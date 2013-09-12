#ifndef GRAPHICS_SIMULATOR_H
#define GRAPHICS_SIMULATOR_H


#include "includes.h"
#include "defines.h"


#include "socket.h"
#include "tools.h"
#include "network.h"
#include "window.h"

#if defined (_WIN32)
	#include <dos.h>
#else
	#include <unistd.h>
#endif



class GraphicsSimulator
{
private:
	SDL_Event event;
	Window window;
	
	int nb_neurons;
	
	// Simulation parameters
	double sim_time;
	int simulation_step;
	int init_time;
	bool stopped;

	// keyboard handling
	bool plus_pressed;
	bool minus_pressed;
	bool p_pressed;
	
	Camera camera;


	void init_connection(int port_send, int port_receive);
	
	void receive_positions();
	void receive_connections();


	void incrementSimulationStep();
	void decrementSimulationStep();

	int handleEvents();


public:
	// Connection parameters
	Socket listener;
	Socket sender;
	// Network and simulation parameters
	Neuron* neurons;
	double curr_time;

	
	void initialize(int port_send, int port_receive, int window_width, int window_height);
	
	void start();
	
	void finalize();
};

void* detect_spikes(void* simulator);

#endif
