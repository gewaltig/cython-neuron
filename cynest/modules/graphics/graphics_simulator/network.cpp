#include "network.h"


// neuron

Neuron::Neuron(int id_, double x_, double y_, double z_):
	id(id_),
	position(x_, y_, z_) {
		connections = vector<int>();
		alpha = ALPHA_THRESHOLD;
		pthread_mutex_init (&mutex , NULL );
		spike_time = 0.0;
		
		double n = generateRandomNumber(1, 100);
		if(n >= 10.0 && n < 11.0) {
			selected = true;
		} else {
			selected = false;
		}
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

int Neuron::getId(){
	return id;
}

Vector3d Neuron::getPosition(){
	return position;
}


double Neuron::getAlpha() {
	return alpha;
}

void Neuron::draw() {
	if(id != 0) {
		if(selected) {
			glColor4f(1.0,0.0,0.0, alpha );
		} else {
			glColor4f(1.0,1.0,1.0, alpha );
		}

		glVertex3f(position.x(), position.y(), position.z());
	}
}



void Neuron::fire(double time_) {
	pthread_mutex_lock (&mutex);
	spikes_buffer.push_back(time_);
	pthread_mutex_unlock (&mutex);
}

void Neuron::update(double time_) {
	if(id != 0) {
		double item = -1.0;
		pthread_mutex_lock (&mutex);
		
		alpha = -(time_ - spike_time) + 1.0;
		
		if(alpha < ALPHA_THRESHOLD) {
			alpha = ALPHA_THRESHOLD;
		}

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
}


void Neuron::select() {
	selected = true;
}

void Neuron::unselect() {
	selected = false;
}

bool Neuron::isSelected() {
	return selected;
}










// camera

void Camera::init() {
	// display parameters initialization
	theta   = 0.0;
	phi     = 0.0;
	dist = 60.0;
}

void Camera::update() {
	pos.set(dist * cos(theta) * cos(phi),   dist * sin(theta) * cos(phi),   dist * sin(phi));
	gluLookAt(pos.x(), pos.y(), pos.z(),  0.0, 0.0, 0.0,  0.0, 0.0, 1.0);
}


void Camera::up() {
	phi = phi + ANGLE_DIFF;
}

void Camera::down() {
	phi = phi - ANGLE_DIFF;
}

void Camera::right() {
	theta = theta + ANGLE_DIFF;
}

void Camera::left() {
	theta = theta - ANGLE_DIFF;
}

void Camera::forward() {
	if (dist - DIST_DIFF >= 0 ) {
		dist -= DIST_DIFF;
	}
}

void Camera::backward() {
	dist += DIST_DIFF;
}




