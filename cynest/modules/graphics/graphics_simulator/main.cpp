#include "graphics_simulator.h"

#define SEND_PORT 50001
#define RCV_PORT 50002
#define WIDTH 1024
#define HEIGHT 680
#define CAPTION "CyNEST Graphics Simulator"


int main() {
	GraphicsSimulator simulator = GraphicsSimulator();
	
	simulator.initialize(SEND_PORT, RCV_PORT, WIDTH, HEIGHT, CAPTION);
	
	simulator.start();
	
	simulator.finalize();

	return 0;
}









