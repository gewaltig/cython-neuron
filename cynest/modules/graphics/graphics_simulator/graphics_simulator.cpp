#include "graphics_simulator.h"

using namespace std;

// connection management

void GraphicsSimulator::init_connection(int port_send_, int port_receive_) {
	sender.initiateConnection(port_send_, NULL);
	listener.acceptConnection(port_receive_, NULL);
	
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
	printf("Positions received\n");
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
		
		printf("params %s\n", bufferParams);
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
				printf("connections %s\n", bufferConn);
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
			printf("end element\n", bufferParams);
			sender.sendMsg("ok", 2);
		}
	}
	printf("Connections received\n");
}


// window management


void GraphicsSimulator::init_window(int window_width_, int window_height_) {
	width = window_width_;
	height = window_height_;
}



// simulation management
void GraphicsSimulator::start(){}


// init and end

void GraphicsSimulator::initialize(int port_send, int port_receive, int window_width, int window_height) {
	init_connection(port_send, port_receive);
	init_window(window_width, window_height);
}

void GraphicsSimulator::finalize() {
	listener.destroy();
	sender.destroy();
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
