#ifndef NETWORK_H
#define NETWORK_H


#include "includes.h"
#include "defines.h"

#include "tools.h"


class Neuron
{
private :
	int id;
	Vector3d position;
	double alpha;
	Vector3d pos_2d;
	vector<double> spikes_buffer;
	double spike_time;
	pthread_mutex_t mutex;
	bool selected;
	
	vector<int> connections;
	
public:
	Neuron();
	Neuron(int id_, double x_, double y_, double z_);

	
	void addConnection(int id_);
	
	vector<int>* getConnections();
	int getId();
	double getAlpha();
	Vector3d getPosition();
	
	void fire(double time_);
	
	void update(double time_);
	
	void draw();
	
	void select();
	void unselect();
	bool isSelected();
};


class Camera
{
private:
	double theta;
	double phi;
	double dist;
	
	Vector3d pos;
public:
	void init();
	void update();
	
	void up();
	void down();
	void right();
	void left();
	void forward();
	void backward();
};



#endif


