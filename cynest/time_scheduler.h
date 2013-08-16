#ifndef TIMESCHEDULER_H
#define TIMESCHEDULER_H

#include "../nestkernel/nest_time.h"
#include "../nestkernel/scheduler.h"

using namespace nest;

class UnitManager {
private:
	/*
	 1 tic
	 2 step
	 3 ms
	 4 ms_stamp
	*/
	int unit;
	Time::tic* tic_temp;
	Time::step* step_temp;
	Time::ms* ms_temp;
	Time::ms_stamp* ms_stamp_temp;

public:
	UnitManager(int u, double value) {
		this->unit = u;
		this->tic_temp = NULL;
		this->step_temp = NULL;
		this->ms_temp = NULL;
		this->ms_stamp_temp = NULL;

		if(u == 3) {
			this->ms_temp = &(Time::ms(value));
		} else if(u == 4) {
			this->ms_stamp_temp = &(Time::ms_stamp(value));
		}
	}

	UnitManager(int u, long value) {
		UnitManager(u, (double)value);

		if(u == 1) {
			this->tic_temp = &(Time::tic(value));
		} else if(u == 2) {
			this->step_temp = &(Time::step(value));
		}
	}

	Time generateTime() {
		if(this->unit == 1) {
			return Time(*(this->tic_temp));
		} else if(this->unit == 2) {
			return Time(*(this->step_temp));
		} else if(this->unit == 3) {
			return Time(*(this->ms_temp));
		} else if(this->unit == 4) {
			return Time(*(this->ms_stamp_temp));
		}
		return Time(Time::ms(0.0));
	}
};

#endif
