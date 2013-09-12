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
	pos.set(0.0, 60.0, 0.0);
	lookAtVector.set(0.0, 0.0, 0.0);
	mode = MODE_CENTERED;
}

void Camera::update() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	switch(mode) {
	case MODE_FREE:
		lookAtVector = pos.add(Vector3d(cos(theta) * cos(phi),   sin(theta) * cos(phi),   sin(phi)));
		break;
	case MODE_CENTERED:
		pos.set(pos.norm() * cos(theta) * cos(phi),   pos.norm() * sin(theta) * cos(phi),   pos.norm() * sin(phi));
		break;
	}

	gluLookAt(pos.x(), pos.y(), pos.z(),  lookAtVector.x(), lookAtVector.y(), lookAtVector.z(),  0.0, 0.0, 1.0);
}

void Camera::setMode(int mode_) {
	Vector3d dir = lookAtVector.sub(pos).normalize();
	
	switch(mode_) {
	case MODE_FREE:
		phi = asin(dir.z() / dir.norm());
		theta = atan(dir.y() / dir.x());
		
		if(dir.x() < 0) {
			theta = fmod(theta + M_PI, 2*M_PI);
		}
		
		break;
	case MODE_CENTERED:
		phi = asin(pos.z() / pos.norm());
		theta = atan(pos.y() / pos.x());
		
		if(pos.x() < 0) {
			theta = fmod(theta + M_PI, 2*M_PI);
		}
		
		lookAtVector.set(0.0, 0.0, 0.0);
		break;
	}
	
	mode = mode_;
}

void Camera::up() {
	if(phi < (M_PI / 2) - ANGLE_DIFF) {
		phi = phi + ANGLE_DIFF;
	}
}

void Camera::down() {
	if(phi > -(M_PI / 2) + ANGLE_DIFF) {
		phi = phi - ANGLE_DIFF;
	}
}

void Camera::right() {
	switch(mode) {
	case MODE_FREE:
		theta = fmod(theta - ANGLE_DIFF, 2*M_PI);
		break;
	case MODE_CENTERED:
		theta = fmod(theta + ANGLE_DIFF, 2*M_PI);
		break;
	}
}

void Camera::left() {
	switch(mode) {
	case MODE_FREE:
		theta = fmod(theta + ANGLE_DIFF, 2*M_PI);
		break;
	case MODE_CENTERED:
		theta = fmod(theta - ANGLE_DIFF, 2*M_PI);
		break;
	}
}

void Camera::forward() {
	Vector3d dir = lookAtVector.sub(pos).normalize();
	
	switch(mode) {
	case MODE_FREE:
		pos = pos.add(dir.mul(DIST_DIFF));
		lookAtVector = lookAtVector.add(dir.mul(DIST_DIFF));
		break;
	case MODE_CENTERED:
		if (pos.norm() > DIST_DIFF ) {
			pos = pos.add(dir.mul(DIST_DIFF));
		}
		break;
	}
}

void Camera::backward() {
	Vector3d dir = lookAtVector.sub(pos).normalize();
	
	switch(mode) {
	case MODE_FREE:
		pos = pos.sub(dir.mul(DIST_DIFF));
		lookAtVector = lookAtVector.sub(dir.mul(DIST_DIFF));
		break;
	case MODE_CENTERED:
		pos = pos.sub(dir.mul(DIST_DIFF));
		break;
	}
}




