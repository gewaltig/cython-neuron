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

double GraphicsSimulator::generateRandomNumber(double low, double high) {
	return (high-low)*((float)rand()/RAND_MAX) + low;
}

void GraphicsSimulator::receive_positions() {
	char buffer[50];
	double pos[4];
	int max = 0;

	while(true){
		listener.receiveMsg(buffer, 50);

		if(strcmp(buffer, "end") == 0) {
			sender.sendMsg("ok", 2);
			break;
		}
		else { // it's a position
			if(parseList(buffer, pos)) {	
				if((int)pos[0] > max) {
					max = (int)pos[0];
				}			
				neurons_.push_back(Neuron((int)pos[0], pos[1], pos[2], pos[3]));
			}
			else {
				if((int)pos[0] > max) {
					max = (int)pos[0];
				}
				neurons_.push_back(Neuron((int)pos[0], generateRandomNumber(RANDOM_POS_LOW, RANDOM_POS_HIGH), generateRandomNumber(RANDOM_POS_LOW, RANDOM_POS_HIGH), generateRandomNumber(RANDOM_POS_LOW, RANDOM_POS_HIGH)));
			}
			sender.sendMsg("ok", 2);
		}
	}

	// array creation
	neurons = new Neuron[max + 1];
	memset(neurons, 0, (max+1)*sizeof(Neuron));
	for(int i = 0; i < neurons_.size(); i++) {
		neurons[neurons_.at(i).getId()] = neurons_.at(i);
	}
	nb_neurons = max + 1;
	neurons_.clear();
}

void GraphicsSimulator::receive_connections() {
	char bufferParams[50];
	double params[3];
	
	char* bufferConn;
	int lengthConn;
	double* conn;
	int nbConn;
	
	while(true){
		listener.receiveMsg(bufferParams, 50);
		
		if(strcmp(bufferParams, "end") == 0) {
			sender.sendMsg("ok", 2);
			break;
		}
		else { // it's a connection
			if(parseList(bufferParams, params)) {
				nbConn = (int)params[1];
				lengthConn = (int)params[2];
				bufferConn = new char[lengthConn + 1];
				conn = new double[nbConn];
				
				sender.sendMsg("param_ok", 8);
				
				listener.receiveMsg(bufferConn, lengthConn + 1);

				if(parseList(bufferConn, conn)) {
					for(int i = 0; i < nbConn; i++) {
						neurons[(int)params[0]].addConnection((int)conn[i]);
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
	window.init(window_width, window_height, caption, neurons, &nb_neurons);
}











// simulation management
void GraphicsSimulator::start() {
	pthread_t spike_detector;
	pthread_create(&spike_detector, NULL, detect_spikes, (void*)this);
	
	
	sender.sendMsg("simulate", 8);
	int eventType = EVENT_NOTHING;

	do
	{
		eventType = window.handleEvents();
		
		switch(eventType)
		{
		case EVENT_QUIT:
			sender.sendMsg("quit", 4);
		}
		
		window.update();
		window.draw();
	} while(eventType != EVENT_QUIT);
	pthread_cancel(spike_detector);
}




void* detect_spikes(void* simulator_) {
	GraphicsSimulator* simulator = (GraphicsSimulator*) simulator_;
	char bufferParams[50];
	double params[2];
	
	char* bufferSpikes;
	int lengthSpikes;
	double* spikes;
	int nbSpikes;
	
	while(true){
		simulator->listener.receiveMsg(bufferParams, 50);
		
		if(strcmp(bufferParams, "finish") == 0) {
			simulator->sender.sendMsg("ok", 2);
			break;
		}
		else { // it's a spike train
			if(parseList(bufferParams, params)) {
				nbSpikes = (int)params[0];
				lengthSpikes = (int)params[1];
				bufferSpikes = new char[lengthSpikes + 1];
				spikes = new double[nbSpikes];
				
				simulator->sender.sendMsg("ok", 2);
				
				simulator->listener.receiveMsg(bufferSpikes, lengthSpikes + 1);

				if(parseList(bufferSpikes, spikes)) {
					for(int i = 0; i < nbSpikes / 2; i++) {
						simulator->neurons[(int)spikes[i]].fire(spikes[i + nbSpikes / 2]);
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
	/* initialize random seed: */
	srand(time(NULL));
}

void GraphicsSimulator::finalize() {
	delete[] neurons;
	listener.destroy();
	sender.destroy();
	window.destroy();
}
