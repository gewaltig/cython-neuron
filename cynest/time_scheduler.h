#ifndef TIMESCHEDULER_H
#define TIMESCHEDULER_H

#include "../nestkernel/nest_time.h"
#include "../nestkernel/scheduler.h"

using namespace nest;


class TimeScheduler {
private:
	/*
	 0 get_resolution
	 1 tic
	 2 step
	 3 ms
	 4 ms_stamp
	*/

	Time createTime(int inputType, long longInputValue, double doubleInputValue) {
	    Time t;

	    if (inputType == 0) { // get_resolution
		t = Time::get_resolution();
	    }
	    else if (inputType == 1){ // tic
		t = Time(Time::tic(longInputValue));
	    }
	    else if (inputType == 2){ // step
		t = Time(Time::step(longInputValue));
	    }
	    else if (inputType == 3){ // ms
		t = Time(Time::ms(doubleInputValue));
	    }
	    else if (inputType == 4) {// ms_stamp
		t = Time(Time::ms_stamp(doubleInputValue));
	    }

	    return t;
	}

public:
	double get_ms(int inputType, long longInputValue, double doubleInputValue) {
	    Time t = createTime(inputType, longInputValue, doubleInputValue);
	    return t.get_ms();
	}


	long get_tics_or_steps(int inputType, int outputType, long longInputValue, double doubleInputValue) {
	    Time t = createTime(inputType, longInputValue, doubleInputValue);

	    if (outputType == 1) {// get tics
		return t.get_tics();
	    }
	    else if (outputType == 2){ // get steps
		return t.get_steps();
	    }

	    return -1;
	}


	/*
	 0 get_modulo
	 1 get_slice_modulo
	 2 get_min_delay
	 3 get_max_delay
	*/
	unsigned int get_scheduler_value(int outputValue, unsigned int arg) {
	    if (outputValue == 0) {
		return Scheduler::get_modulo(arg);
	    }
	    else if (outputValue == 1){
		return Scheduler::get_slice_modulo(arg);
	    }
	    else if (outputValue == 2){
		return Scheduler::get_min_delay();
	    }
	    else if (outputValue == 3){
		return Scheduler::get_max_delay();
	    }

	    return -1;
	}

};
#endif
