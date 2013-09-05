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


class GraphicsSimulator
{
private:
	Socket listener;
	Socket sender;
	int width;
	int height;
	vector<Neuron> neurons;
	
	void init_connection(int port_send_, int port_receive_);
	void init_window(int window_width_, int window_height_);
	
	void receive_positions();
	void receive_connections();
	
	int getIndexFromId(int id);
	
public:
	void initialize(int port_send, int port_receive, int window_width, int window_height);
	
	void start();
	
	void finalize();
};


#endif
