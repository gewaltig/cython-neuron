#ifndef DEFINES_H
#define DEFINES_H


// General definitions
#define DEF_SEND_PORT 50001
#define DEF_RCV_PORT 50002
#define WIDTH 1024
#define HEIGHT 680


// Neuron definitions
#define ALPHA_THRESHOLD 0.2
#define ALPHA_COEFF 0.5


// Camera definitions
#define ANGLE_DIFF 0.05
#define DIST_DIFF 1.0
#define PI 3.15


// Simulation definitions
#define LOW_BOUND_SIM_STEP 1
#define HIGH_BOUND_SIM_STEP 10000
#define SIMULATION_DELTA 1
#define INITIAL_SIMULATION_STEP 100


// Network definitions
#define RANDOM_POS_LOW -20.0
#define RANDOM_POS_HIGH 20.0


// Other definitions
#define LIST_ELEMENT_SIZE 20


// Event definitions
#define EVENT_NOTHING 0
#define EVENT_QUIT 1
#define EVENT_STOP 2
#define EVENT_RESUME 3
#define EVENT_STEP_CHANGED 4
#define EVENT_PAUSE 5
#define EVENT_RESUME 6

#endif
