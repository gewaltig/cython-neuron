#include "window.h"

using namespace std;

void Window::init(int width_, int height_, char* caption, Neuron* neurons_, int* nb_neurons_) {
	neurons = neurons_;
	nb_neurons = nb_neurons_;
	width = width_;
	height = height_;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	surface = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_DOUBLEBUF );
		
	SDL_WM_SetCaption(caption, 0);
			
	// display parameters initialization
	theta   = 0.0;
	phi     = 0.0;
	camera_dist = 30.0;
	    
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
 
	gluPerspective(45.0, float(width)/float(height), 1.0, 100.0);
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
		}
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
	}
	
	return EVENT_NOTHING;
}

void Window::update(double time_) {
	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0)  {
			neurons[i].update(time_);
		}
	}
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


void Window::draw(double time_) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// camera coordinates
	camera_pos.set(camera_dist * cos(theta) * cos(phi),   camera_dist * sin(theta) * cos(phi),   camera_dist * sin(phi));
	gluLookAt(camera_pos.x(), camera_pos.y(), camera_pos.z(),  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
	
	
	glPushMatrix();
	// draw points (these should be drawn nicer using shaders)
	glPointSize(3.0);
	glBegin(GL_POINTS);

	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0)  {
			neurons[i].draw(time_);
		}
	}
	glEnd();
	glPopMatrix();
	
	draw_connections();
	
	glFlush();
	SDL_GL_SwapBuffers();
}
