#include "window.h"

using namespace std;

void Window::init(int width_, int height_, Neuron* neurons_, int* nb_neurons_, int* sim_step, double* sim_time_) {
	neurons = neurons_;
	nb_neurons = nb_neurons_;
	width = width_;
	height = height_;
	simulation_step = sim_step;
	simulation_total_time = sim_time_;
	w_pressed = false;
	s_pressed = false;
	p_pressed = false;
	stopped = false;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	TTF_Init();
	
	surface = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_DOUBLEBUF );

	// display parameters initialization
	theta   = 0.0;
	phi     = 0.0;
	camera_dist = 60.0;
	    
    init_display();
    init_neuron_params();
    
    // background
	glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Window::init_display() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
 
	gluPerspective(45.0, float(width)/float(height), 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void Window::resize(int width_, int height_) {
	width = width_;
	height = height_;
	glViewport(0, 0, width, height);
	
	init_display();
}


void Window::init_neuron_params() {	
	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0) {
			neurons[i].setCameraPosition(&camera_pos);
		}
	}
}



void Window::destroy() {
	TTF_Quit();
	SDL_Quit();
}



// Handles events and returns
// the event type
// rotation has to be improved
int Window::handleEvents() {
	SDL_PollEvent(&event);
	
	switch(event.type)
	{
	case SDL_QUIT:
		return EVENT_QUIT;
		break;
	case SDL_KEYDOWN:
		switch(event.key.keysym.sym)
		{
		case SDLK_UP:
			phi = phi + ANGLE_DIFF;
			break;
		case SDLK_DOWN:
			phi = phi - ANGLE_DIFF;
			break;
		case SDLK_LEFT:
			theta = theta - ANGLE_DIFF;
			break;
		case SDLK_RIGHT:
			theta = theta + ANGLE_DIFF;
			break;
		case SDLK_w:
			if(!w_pressed) {
				w_pressed = true;
				
				if(!stopped) {
					incrementSimulationStep(simulation_step);
					return EVENT_STEP_CHANGED;
				}
			}
			break;
		case SDLK_s:
			if(!s_pressed) {
				s_pressed = true;
				
				if(!stopped) {
					decrementSimulationStep(simulation_step);
					return EVENT_STEP_CHANGED;
				}
			}
			break;	
		case SDLK_p:
			if(!p_pressed) {
				p_pressed = true;
				stopped = !stopped;
				if(!stopped) {
					return EVENT_RESUME;
				} else {
					return EVENT_STOP;
				}
			}
			break;
		}
		break;
	case SDL_KEYUP:
		switch(event.key.keysym.sym)
		{
		case SDLK_w:
			if(w_pressed) {
				w_pressed = false;
			}
			break;
		case SDLK_s:
			if(s_pressed) {
				s_pressed = false;
			}
			break;
		case SDLK_p:
			if(p_pressed) {
				p_pressed = false;
			}
			break;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		switch(event.button.button) {
		case SDL_BUTTON_WHEELUP:
			if (camera_dist - DIST_DIFF >= 0 ) {
				camera_dist -= DIST_DIFF;
			}
			break;
		case SDL_BUTTON_WHEELDOWN:
			camera_dist += DIST_DIFF;
			break;
		}
		break;
	}
	
	return EVENT_NOTHING;
}

void Window::update(double time_) {
	double percentage = 100.0;
	if(time_ < *simulation_total_time) {
		percentage = time_ * 100.0 / *simulation_total_time;
	}
	
	sprintf(caption, "CyNEST Graphics Simulator - State: %.1f%, RT Factor: 1/%d",  percentage, *simulation_step);
	SDL_WM_SetCaption(caption, 0);
}


void Window::draw_connections() {
	glPushMatrix();

	glBegin(GL_LINES);
	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0) {
			if(neurons[i].getAlpha() > ALPHA_THRESHOLD) { // activity
				glColor4f(1.0,1.0,1.0, neurons[i].getAlpha());
			} else { // inactivity
				glColor4f(1.0,1.0,1.0, ALPHA_THRESHOLD);
			}
			
			Vector3d src = neurons[i].getPosition();
			vector<int>* connections = neurons[i].getConnections();
			for(int j = 0; j < connections->size(); j++) {
				Vector3d dest = neurons[connections->at(j)].getPosition();

				glVertex3f(src.x(),src.y(),src.z());
				glVertex3f(dest.x(),dest.y(),dest.z());
			}
		}
	}
	glEnd();
	glPopMatrix();
}


void Window::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// camera coordinates
	camera_pos.set(camera_dist * cos(theta) * cos(phi),   camera_dist * sin(theta) * cos(phi),   camera_dist * sin(phi));
	gluLookAt(camera_pos.x(), camera_pos.y(), camera_pos.z(),  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
	
	
	// draw points (these should be drawn nicer using shaders)
	glPushMatrix();
	glPointSize(3.0);
	glBegin(GL_POINTS);

	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0)  {
			neurons[i].draw();
		}
	}
	glEnd();
	glPopMatrix();
	
	draw_connections();
	
	
	glFlush();
	SDL_GL_SwapBuffers();
}





// others


void incrementSimulationStep(int* v) {
	int tmp = *v;
	int order = 0;
	
	while(tmp >= 1) {
		tmp /= 10;
		order++;
	}
	int inc = pow(10, order - 1);
	
	*v += inc;
	if(*v > HIGH_BOUND_SIM_STEP ) {
		*v = HIGH_BOUND_SIM_STEP;
	}
}



void decrementSimulationStep(int* v) {
	int tmp = *v;
	int order = 0;
	
	while(tmp >= 1) {
		tmp /= 10;
		order++;
	}
	int inc = pow(10, order - 1);
	
	if(*v - inc > 0) {
		*v -= inc;
	} else {
		*v -= inc / 10;
	}
	
	if(*v < LOW_BOUND_SIM_STEP ) {
		*v = LOW_BOUND_SIM_STEP;
	}
}
