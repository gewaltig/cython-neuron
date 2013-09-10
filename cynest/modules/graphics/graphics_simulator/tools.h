#ifndef TOOLS_H
#define TOOLS_H

#define VAL_BUFFER 20
#define ALPHA_THRESHOLD 0.2
#define ALPHA_COEFF 0.5

#define SIMULATION_STEP 100
#define SIMULATION_DELTA 1

#include <GL/glu.h>
#include <SDL/SDL.h>

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <pthread.h>

using namespace std;


class Vector3d {
private:
	double x_;
	double y_;
	double z_;
	double norm_;

public:	
	Vector3d();
	
	Vector3d(double x__, double y__, double z__);
	
	void set(double x__, double y__, double z__);
	
	double norm();
	Vector3d add(Vector3d v);
	Vector3d sub(Vector3d v);
	Vector3d normalize();
	double x();
	double y();
	double z();
};

class Neuron
{
private :
	int id;
	Vector3d position;
	Vector3d* camera_pos;
	double alpha;
	Vector3d pos_2d;
	vector<double> spikes_buffer;
	double spike_time;
	pthread_mutex_t mutex;
	
	vector<int> connections;
	
public:
	Neuron();
	Neuron(int id_, double x_, double y_, double z_);
	
	void setCameraPosition(Vector3d* camera_pos_);
	
	void addConnection(int id_);
	
	vector<int>* getConnections();
	int getId();
	double getAlpha();
	Vector3d getPosition();
	
	void fire(double time_);
	
	void update(double time_);
	
	void draw(double time_);
};

bool parseList(char* str, double* list);

double generateRandomNumber(double low, double high);
#endif


	/*
		void translate_3d_to_2d(double x_3d, double y_3d, double z_3d, double* x_2d, double* y_2d);
// returns a 2D projection position from a 3D position
void Window::translate_3d_to_2d(double x_3d, double y_3d, double z_3d, double* x_2d, double* y_2d) {
    /*modelview = glGetDoublev(GL_MODELVIEW_MATRIX);
    projection = glGetDoublev(GL_PROJECTION_MATRIX );
    viewport = glGetIntegerv(GL_VIEWPORT );
    P2D = GLU.gluProject(pos_[0],pos_[1],pos_[2],modelview,projection,viewport);
    return np.array([-1.0 + 2.0*P2D[0]/Wreal, -1.0 + 2.0*P2D[1]/Hreal, P2D[2]])
}*/


