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
		alpha = ALPHA_THRESHOLD;
		pthread_mutex_init (&mutex , NULL );
		spike_time = 0.0;
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


double Neuron::getAlpha() {
	return alpha;
}

void Neuron::draw(double time_) {
	alpha = -(time_ - spike_time) + 1.0;
	
	if(alpha < ALPHA_THRESHOLD) {
		alpha = ALPHA_THRESHOLD;
	}
	
	glColor4f(1.0,1.0,1.0, alpha );

	glVertex3f(position.x(), position.y(), position.z());
}



void Neuron::fire(double time_) {
	pthread_mutex_lock (&mutex);
	spikes_buffer.push_back(time_);
	pthread_mutex_unlock (&mutex);
}

void Neuron::update(double time_) {
	double item = -1.0;
	pthread_mutex_lock (&mutex);

	while(spikes_buffer.size() > 0 && item < time_) {	
		item = spikes_buffer.at(0);
		
		spikes_buffer.erase (spikes_buffer.begin());
	}
	
	if(item != -1.0) {
		if(item >= time_ && item < time_ + SIMULATION_DELTA) {
			spike_time = item;
			alpha = 1.0; // fire
		}	
		else { // we put it inside again
			spikes_buffer.insert (spikes_buffer.begin(), item);
		}
	}
	pthread_mutex_unlock (&mutex);
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



double generateRandomNumber(double low, double high) {
	return (high-low)*((float)rand()/RAND_MAX) + low;
}

