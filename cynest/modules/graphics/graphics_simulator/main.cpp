#include "graphics_simulator.h"

#define SEND_PORT 50001
#define RCV_PORT 50002


int main() {
	GraphicsSimulator simulator = GraphicsSimulator();
	
	simulator.initialize(SEND_PORT, RCV_PORT, 1024, 680);
	
	simulator.start();
	
	simulator.finalize();

	return 0;
}









