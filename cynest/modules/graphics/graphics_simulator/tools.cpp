#include "tools.h"

// vector 3d
Vector3d::Vector3d(double x__, double y__, double z__) {
	x_ = x__;
	y_ = y__;
	z_ = z__;
	norm_ = sqrt(x_*x_ + y_*y_ + z_*z_);
}

Vector3d::Vector3d() {
	Vector3d(0.0, 0.0, 0.0);
}

void Vector3d::set(double x__, double y__, double z__) {
	x_ = x__;
	y_ = y__;
	z_ = z__;
	norm_ = sqrt(x_*x_ + y_*y_ + z_*z_);
}

double Vector3d::norm() {
	return norm_;
}

Vector3d Vector3d::add(Vector3d v) {
	return Vector3d(x_ + v.x_, y_ + v.y_, z_ + v.z_);
}

Vector3d Vector3d::sub(Vector3d v) {
	return Vector3d(x_ - v.x_, y_ - v.y_, z_ - v.z_);
}

Vector3d Vector3d::normalize() {
	double n = norm();

	return Vector3d(x_ / n, y_ / n, z_ / n);
}

double Vector3d::x() {
	return x_;
}

double Vector3d::y() {
	return y_;
}

double Vector3d::z() {
	return z_;
}



// neuron

Neuron::Neuron(int id_, double x_, double y_, double z_):
	id(id_),
	position(x_, y_, z_) {
		connections = vector<int>();
		fires = false;
		alpha = 0.5;
}

Neuron::Neuron() {
	Neuron(0, 0.0, 0.0, 0.0);
}

void Neuron::addConnection(int id_) {
	connections.push_back(id_);
}

vector<int>* Neuron::getConnections(){
	return &connections;
}


void Neuron::setCameraPosition(Vector3d* camera_pos_) {
	camera_pos = camera_pos_;
}

int Neuron::getId(){
	return id;
}

Vector3d Neuron::getPosition(){
	return position;
}

void Neuron::fire(double time) {
	fires = true;
	fire_time = time;
}

double Neuron::getAlpha() {
	return alpha;
}

void Neuron::draw() {
	double dist_factor = camera_pos->norm() / camera_pos->sub(position).norm(); // have to change 30.0
	glColor4f(1.0,1.0,1.0, 0.6 * dist_factor    + alpha * 0.3 );

	glVertex3f(position.x(), position.y(), position.z());
}

void Neuron::update(double time) {
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




