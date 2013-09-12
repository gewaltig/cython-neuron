#include "window.h"

using namespace std;

void Window::init(int width_, int height_, Neuron* neurons_, int* nb_neurons_, int* sim_step, double* sim_time_) {
	neurons = neurons_;
	nb_neurons = nb_neurons_;
	width = width_;
	height = height_;
	simulation_step = sim_step;
	simulation_total_time = sim_time_;
	plus_pressed = false;
	minus_pressed = false;
	p_pressed = false;
	stopped = false;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	TTF_Init();
	
	surface = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_DOUBLEBUF );
	    
    init_display();
    
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
	camera.init();
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
			camera.up();
			break;
		case SDLK_DOWN:
			camera.down();
			break;
		case SDLK_LEFT:
			camera.left();
			break;
		case SDLK_RIGHT:
			camera.right();
			break;
		case SDLK_KP_MINUS:
			if(!minus_pressed) {
				minus_pressed = true;
				
				if(!stopped) {
					incrementSimulationStep(simulation_step);
					return EVENT_STEP_CHANGED;
				}
			}
			break;
		case SDLK_KP_PLUS:
			if(!plus_pressed) {
				plus_pressed = true;
				
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
		case SDLK_KP_MINUS:
			if(minus_pressed) {
				minus_pressed = false;
			}
			break;
		case SDLK_KP_PLUS:
			if(plus_pressed) {
				plus_pressed = false;
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
			camera.forward();
			break;
		case SDL_BUTTON_WHEELDOWN:
			camera.backward();
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
		if(neurons[i].getId() != 0) {
			Vector3d color = Vector3d(1.0, 1.0, 1.0);
			
			if(neurons[i].isSelected()) {
				color.set(1.0, 0.0, 0.0);
			}

			glColor4f(color.x(), color.y(), color.z(), neurons[i].getAlpha());
			
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
	camera.update();
	
	
	// draw points (these should be drawn nicer using shaders)
	glPushMatrix();
	glPointSize(3.0);
	glBegin(GL_POINTS);

	for(int i=0; i < *nb_neurons; i++) {
		neurons[i].draw();
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
