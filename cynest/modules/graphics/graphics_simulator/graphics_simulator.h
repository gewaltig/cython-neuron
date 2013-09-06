#ifndef GRAPHICS_SIMULATOR_H
#define GRAPHICS_SIMULATOR_H

#include <iostream>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <GL/glu.h>
#include <SDL/SDL.h>


#include "socket.h"
#include "tools.h"
#include "window.h"

#if defined (_WIN32)
	#include <dos.h>
#else
	#include <unistd.h>
#endif


class GraphicsSimulator
{
private:
	Socket listener;
	Socket sender;
	vector<Neuron> neurons;
	Window window;
	
	void init_connection(int port_send, int port_receive);
	void init_window(int window_width, int window_height, char* caption);
	
	void receive_positions();
	void receive_connections();
	
	int getIndexFromId(int id);
	
public:
	void initialize(int port_send, int port_receive, int window_width, int window_height, char* caption);
	
	void start();
	
	void finalize();
};


#endif
