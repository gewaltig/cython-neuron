#ifndef TOOLS_H
#define TOOLS_H

#define VAL_BUFFER 20


#include <vector>
#include <cstdlib>

using namespace std;

class Vector3d {
public:
	double x;
	double y;
	double z;
	
	Vector3d(double x_, double y_, double z_);
};

class Neuron
{
private :
	int id;
	Vector3d position;
	bool has_position;
	
	vector<int> connections;
	
public:
	Neuron(int id_, double x_, double y_, double z_, bool has_position_);
	
	void addConnection(int id_);
	
	vector<int> getConnections();
	Vector3d getPosition();
	int getId();
};

bool parseList(char* str, double* list);

#endif
