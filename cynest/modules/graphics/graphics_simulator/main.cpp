#include "graphics_simulator.h"
#include <cstdio>

#define DEF_SEND_PORT 50001
#define DEF_RCV_PORT 50002
#define WIDTH 1024
#define HEIGHT 680


int main(int argc, char *argv[]) {
	srand(time(NULL));
	GraphicsSimulator simulator = GraphicsSimulator();
	
	if(argc == 1) {
		simulator.initialize(DEF_SEND_PORT, DEF_RCV_PORT, WIDTH, HEIGHT);
	}
	else if (argc == 2) {
		int port = atof(argv[1]);
		simulator.initialize(port, port + 1, WIDTH, HEIGHT);
	}
	else {
		printf("Error: wrong argument number");
		return 1;
	}
	simulator.start();
	
	simulator.finalize();

	return 0;
}









