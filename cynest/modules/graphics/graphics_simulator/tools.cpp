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




string deleteExecName(string path) {
        int slashPointer = path.length() - 1;
        while(slashPointer >= 0 && path[slashPointer] != '/') {
                slashPointer--;
        }
        return path.substr(0, slashPointer + 1);
}

string getExecDirectory() {
  char pBuf[FILENAME_MAX];
#ifdef WINDOWS
        int bytes = GetModuleFileName(NULL, pBuf, FILENAME_MAX);
        if(bytes == 0)
                return NULL;
        else
                return deleteExecName(string(pBuf));

#else
        char szTmp[32];
        sprintf(szTmp, "/proc/%d/exe", getpid());
        int bytes = min((int)readlink(szTmp, pBuf, FILENAME_MAX), FILENAME_MAX - 1);
        if(bytes >= 0)
                pBuf[bytes] = '\0';
        return deleteExecName(string(pBuf));
#endif
}

