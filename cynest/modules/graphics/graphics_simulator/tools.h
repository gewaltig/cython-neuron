#ifndef TOOLS_H
#define TOOLS_H

#define VAL_BUFFER 20
#define ALPHA_THRESHOLD 0.2
#define ALPHA_COEFF 0.5

#define SIMULATION_DELTA 1

#include <GL/glu.h>
#include <SDL/SDL.h>
#include "SDL_ttf.h"

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <pthread.h>
#include <string>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

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
	
	void draw();
};

bool parseList(char* str, double* list);

double generateRandomNumber(double low, double high);

string getExecDirectory();
string deleteExecName(string path);

#endif


