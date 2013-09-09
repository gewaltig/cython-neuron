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

void Window::update() {
	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0)  {
			neurons[i].update(0.0);
		}
	}
}


void Window::draw_connections() {	
	glPushMatrix();
	// draw connections
	glBegin(GL_TRIANGLES);
	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0) {
			glColor4f(0.0,1.0,0.0, 0.2 + neurons[i].getAlpha()*0.3);
			Vector3d src = neurons[i].getPosition();
			vector<int>* connections = neurons[i].getConnections();
			for(int j = 0; j < connections->size(); j++) {
				Vector3d dest = neurons[connections->at(j)].getPosition();
				Vector3d direction  = dest.sub(src).normalize();
				Vector3d normal1 = Vector3d(0.0, 0.0, 1.0);
				Vector3d normal2 = Vector3d(0.0, 1.0, 0.0);
				normal1.set(direction.z()/direction.x(), normal1.y(), normal1.z());
				normal2.set(direction.y()/direction.x(), normal2.y(), normal2.z());
				normal1 = normal1.normalize();
				normal2 = normal2.normalize();   
				double coef = 0.1;
				glVertex3f(src.x() + coef*normal1.x(), src.y() + coef*normal1.y(), src.z() + coef*normal1.z());
				glVertex3f(src.x() + coef*normal2.x(), src.y() + coef*normal2.y(), src.z() + coef*normal2.z());
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
	
	// Drawing neurons
	glPushMatrix();
         
	// draw points (these should be drawn nicer using shaders)
	glPointSize(5.0);
	glBegin(GL_POINTS);

	for(int i=0; i < *nb_neurons; i++) {
		if(neurons + i != 0)  {
			neurons[i].draw();
		}
	}
	glEnd();
	glPopMatrix();
	
	//draw_connections();
	
	glFlush();
	SDL_GL_SwapBuffers();
}




/*
import os
import sys
 
# in case nest is not on the default python path
#~ sys.path.append('../LIB/nest/lib64/python2.6/site-packages')
sys.path.append('../LIB/nest-r9837/lib64/python2.6/site-packages')
import nest
 
from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import QtOpenGL
from OpenGL import GLU
from OpenGL.GL import *
 
import numpy as np
from pylab import *
from time import time, sleep
 
 
# window size
W = 1200.0
H = 800.0
# drawable surface size
Wreal = W
Hreal = H
 
# view angles and camera distance
theta  = 0.0
phi    = 0.0
camdist= 5.0
 
# fabulous texture for the brilliant neurons
auratex = -1
 
# neuron 3D positions
pos   = []
# neuron 2D positions (projection on the screen)
pos2d = []
# alpha value of neurons (is increased by each spike, decreases after each spike)
val   = []
 
 
 
 
 	void translate_3d_to_2d(double x_3d, double y_3d, double z_3d, double* x_2d, double* y_2d);
 
# returns a 2D projection position from a 3D position
def get_2D_pos_from_3D(pos_):
    modelview = glGetDoublev(GL_MODELVIEW_MATRIX);
    projection = glGetDoublev(GL_PROJECTION_MATRIX );
    viewport = glGetIntegerv(GL_VIEWPORT );
    P2D = GLU.gluProject(pos_[0],pos_[1],pos_[2],modelview,projection,viewport);
    return np.array([-1.0 + 2.0*P2D[0]/Wreal, -1.0 + 2.0*P2D[1]/Hreal, P2D[2]])
 
 
 
# returns a 3D position from a 2D projection position, will be used to create neurons from a mouse click
#~ V3 get_3D_pos_from_2D(V3 blendtarget, V3 camera_rl, V3 camera_ud, V3 camera_pos, V2 pos3D, int W, int H, float FOV){
#~
#~ return blendtarget + camera_rl*(float(W)/float(H))*(tan((FOV/2.0)*M_PI/180.0))*(blendtarget-camera_pos).norm()*2.0*((float(pos3D.x))) - camera_ud*(tan((FOV/2.0)*M_PI/180.0))*(blendtarget-camera_pos).norm()*2.0*((float(pos3D.y)));
#~
#~ }
 

# give'em random external currents
nest.SetStatus(range(1,len(pos)+1), "I_e", list(750.0 + np.random.rand(len(pos))*600.0))
 
 
 

    # draw !!!
    def paintGL(self):
         
        nest.Simulate(0.1)
        Vs = nest.GetStatus(range(1,len(pos)+1), "V_m")
         
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
         
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        # camera coordinates
        GLU.gluLookAt(camdist * cos(theta) * cos(phi), camdist * sin(theta) * cos(phi), camdist * sin(phi),  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
        
        glPushMatrix()
         
        # draw points (these should be drawn nicer using shaders)
        glBindTexture(GL_TEXTURE_2D, 0)
        glPointSize(10.0)
        glBegin(GL_POINTS)
        for ip,p in enumerate(pos):
            glColor4f(1.0,1.0,1.0, 0.2 + val[ip]*0.3)
            glVertex3f(p[0],p[1],p[2])
        glEnd()
        glPopMatrix()
         
         
        # draw brilliance auras
        glPushMatrix()
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix()
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        Bsize = 0.32
        glBindTexture(GL_TEXTURE_2D, texx)
        glBegin(GL_QUADS)
        for ip,p2d in enumerate(pos2d):
            global Wreal, Hreal
            HW = float(Hreal)/float(Wreal)
            scneu = (Vs[ip] + 65.0) / 40.0
            glColor4f(1.0,1.0,1.0, val[ip]*0.3)
            glTexCoord2d(0.0,0.0)
            glVertex2f(p2d[0]-Bsize*HW, p2d[1]-Bsize)
            glTexCoord2d(1.0,0.0);
            glVertex2f(p2d[0]+Bsize*HW, p2d[1]-Bsize)
            glTexCoord2d(1.0,1.0);
            glVertex2f(p2d[0]+Bsize*HW, p2d[1]+Bsize)
            glTexCoord2d(0.0,1.0);
            glVertex2f(p2d[0]-Bsize*HW, p2d[1]+Bsize)
            val[ip] += (1.0/100.0) * (-val[ip])
            if Vs[ip]>-45.0:val[ip] = 1.0
        glEnd()
        glMatrixMode(GL_PROJECTION);
        glPopMatrix()
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix()
         
        # get 2D projection positions from 3D positions
        for ip,p in enumerate(pos):
            pos2d[ip] = get_2D_pos_from_3D(p)

 
 
 
# texture loading
def TexFromPNG(filename):
    import Image
    img = Image.open(filename)
    img_data = np.array(list(img.getdata()), np.uint16)
    texture = glGenTextures(1)
    glPixelStorei(GL_UNPACK_ALIGNMENT,1)
    glBindTexture(GL_TEXTURE_2D, texture)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.size[0], img.size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)
    return texture
 
 
 
texx = TexFromPNG("aura_round.png")
 
 
sys.exit(app.exec_())


*/
