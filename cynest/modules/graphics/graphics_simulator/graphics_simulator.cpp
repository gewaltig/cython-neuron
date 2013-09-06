#include "graphics_simulator.h"

using namespace std;

// connection management

void GraphicsSimulator::init_connection(int port_send, int port_receive) {
	sender.initiateConnection(port_send, NULL);
	listener.acceptConnection(port_receive, NULL);
	
	sleep(1);
	
	sender.sendMsg("ready", 5);
	
	receive_positions();
	receive_connections();
}

void GraphicsSimulator::receive_positions() {
	char buffer[50];
	double pos[4];
	while(true){
		listener.receiveMsg(buffer, 50);
		
		if(strcmp(buffer, "end") == 0) {
			sender.sendMsg("ok", 2);
			break;
		}
		else { // it's a position
			if(parseList(buffer, pos)) {
				neurons.push_back(Neuron((int)pos[0], pos[1], pos[2], pos[3], true));
			}
			else {
				neurons.push_back(Neuron(pos[0], 0.0, 0.0, 0.0, false));
			}
			sender.sendMsg("ok", 2);
		}
	}
}

void GraphicsSimulator::receive_connections() {
	char bufferParams[50];
	double params[3];
	
	char* bufferConn;
	int lengthConn;
	double* conn;
	int nbConn;
	
	int index;
	
	while(true){
		listener.receiveMsg(bufferParams, 50);
		
		if(strcmp(bufferParams, "end") == 0) {
			sender.sendMsg("ok", 2);
			break;
		}
		else { // it's a connection
			if(parseList(bufferParams, params)) {
				index = getIndexFromId((int)params[0]);
				nbConn = (int)params[1];
				lengthConn = (int)params[2];
				bufferConn = new char[lengthConn + 1];
				conn = new double[nbConn];
				
				sender.sendMsg("param_ok", 8);
				
				listener.receiveMsg(bufferConn, lengthConn + 1);

				if(parseList(bufferConn, conn) && index > -1) {
					for(int i = 0; i < nbConn; i++) {
						neurons.at(index).addConnection((int)conn[i]);
					}
					sender.sendMsg("msg_ok", 6);
				}
				else {
					printf("Problem receiving connections. Program will exit.");
					exit(1);
				}
				
				delete[] bufferConn;
				delete[] conn;
			}
			else {
				printf("Problem receiving connections. Program will exit.");
				exit(1);
			}
			sender.sendMsg("ok", 2);
		}
	}
}

// end of connection management







// window management


void GraphicsSimulator::init_window(int window_width, int window_height, char* caption) {
	window.init(window_width, window_height, caption, &neurons);
}











// simulation management
void GraphicsSimulator::start() {
	pthread_t spike_detector;
	pthread_create(&spike_detector, NULL, detect_spikes, (void*)this);
	
	
	sender.sendMsg("simulate", 8);
	simulation_running = true;
	int eventType = EVENT_NOTHING;

	do
	{
		eventType = window.handleEvents();
		
		switch(eventType)
		{
		case EVENT_QUIT:
			sender.sendMsg("quit", 4);
		}
		
		window.draw();
	} while(eventType != EVENT_QUIT && simulation_running);
}




void* detect_spikes(void* simulator_) {
	GraphicsSimulator* simulator = (GraphicsSimulator*) simulator_;
	char bufferParams[50];
	double params[3];
	
	char* bufferSpikes;
	int lengthSpikes;
	double* spikes;
	int nbSpikes;
	
	double time;
	int index;
	
	while(true){
		simulator->listener.receiveMsg(bufferParams, 50);
		
		if(strcmp(bufferParams, "finish") == 0) {
			simulator->sender.sendMsg("ok", 2);
			simulator->simulation_running = false;
			break;
		}
		else { // it's a spike train
			if(parseList(bufferParams, params)) {
				time = params[0];
				nbSpikes = (int)params[1];
				lengthSpikes = (int)params[2];
				bufferSpikes = new char[lengthSpikes + 1];
				spikes = new double[nbSpikes];
				
				simulator->sender.sendMsg("ok", 2);
				
				simulator->listener.receiveMsg(bufferSpikes, lengthSpikes + 1);

				if(parseList(bufferSpikes, spikes)) {
					for(int i = 0; i < nbSpikes; i++) {
						index = simulator->getIndexFromId((int)spikes[i]);
						simulator->neurons.at(index).fire(time);
					}
				}
				
				delete[] bufferSpikes;
				delete[] spikes;
			}
		}
	}
}








// init and end

void GraphicsSimulator::initialize(int port_send, int port_receive, int window_width, int window_height, char* caption) {
	init_connection(port_send, port_receive);
	init_window(window_width, window_height, caption);
	simulation_running = false;
}

void GraphicsSimulator::finalize() {
	pthread_exit(NULL);
	listener.destroy();
	sender.destroy();
	window.destroy();
}

// others
int GraphicsSimulator::getIndexFromId(int id) {
	for(int i = 0; i < neurons.size(); i++) {
		if(neurons.at(i).getId() == id) {
			return i;
		}
	}
	
	return -1;
}
