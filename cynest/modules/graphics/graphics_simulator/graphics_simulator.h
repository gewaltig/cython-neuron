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
	Window window;
	
	int nb_neurons;
	
	double sim_time;
	int simulation_step;
	int init_time;


	void init_connection(int port_send, int port_receive);
	void init_window(int window_width, int window_height);
	
	void receive_positions();
	void receive_connections();


public:
	Socket listener;
	Socket sender;
	Neuron* neurons;
	double curr_time;

	
	void initialize(int port_send, int port_receive, int window_width, int window_height);
	
	void start();
	
	void finalize();
};

void* detect_spikes(void* simulator);

#endif
