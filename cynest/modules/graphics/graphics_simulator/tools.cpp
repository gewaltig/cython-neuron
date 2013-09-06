#include "tools.h"

// vector 3d
Vector3d::Vector3d(double x_, double y_, double z_) {
	this->x = x_;
	this->y = y_;
	this->z = z_;
}




// neuron

Neuron::Neuron(int id_, double x_, double y_, double z_, bool has_position_):
	id(id_),
	position(x_, y_, z_),
	has_position(has_position_) {
		connections = vector<int>();
		fires = false;
}


void Neuron::addConnection(int id_) {
	connections.push_back(id_);
}

vector<int> Neuron::getConnections(){
	return connections;
}



Vector3d Neuron::getPosition(){
	return position;
}

int Neuron::getId(){
	return id;
}

void Neuron::fire(double time) {
	fires = true;
	fire_time = time;
}

double Neuron::hasFired() {
	bool f = fires;
	fires = false;
	
	if(f) {
		return fire_time;
	}
	return -1.0;
}

// others

// takes a string of the format "[val1,val2,val3,...]"
// and returns a double array of these values
bool parseList(char* str, double* list) {
	char valBuffer[VAL_BUFFER] = {0};
	char* cChar = &str[1]; // current char pointer, neglect the initial '['
	int j = 0; // valBuffer position
	int i = 0; // i: current value (val1, val2, ...)
	
	while(true) { 
		if(*cChar == ' ') {
			// nothing, skip
		}
		else if(*cChar == ',') {
			valBuffer[j] = '\0';
			j = 0;
			list[i] = atof(valBuffer);
			i++;
		}
		else if(*cChar == ']') {
			valBuffer[j] = '\0';
			list[i] = atof(valBuffer);
			break;
		}
		else if(*cChar >= 48 && *cChar <= 57 || *cChar == '-' || *cChar == '.') { // 0 - 9, -, .
			valBuffer[j] = *cChar;
			j++;
		}
		else {
			return false;
		}
		cChar++;
	}
	
	return true;
}









