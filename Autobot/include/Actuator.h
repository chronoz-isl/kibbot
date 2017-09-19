#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>


#include <boost/thread.hpp>

#include <Arbitrator.h>


int Bumper_state = 0;
int Kinect_state = 0;

/* Class which performs actuation (motor turning, led switching, ...). */
class Actuator {
public :
	Actuator(double &vxi, double &vzi, Arbitrator *arb) : vr(vxi),vl(vzi), main_state(arb->main_state()) {}

	void start() {
		_thread = boost::thread(&Actuator::run, this);
	}

	void join() {
		_thread.join();
	}

	void interrupt() {
		_thread.interrupt();
	}

private :
	void run() {
		while (true)
		{
			/*
			char c = cvWaitKey(0);
			if (c == 27) break;
			*/
			
			switch (*main_state)
			{
			case 'w': vl =   +1; vr =   +1; break;  // up
			case 's': vl =   -1; vr =   -1; break;  // down
			case 'a': vl =   -1; vr =   +1; break;  // left
			case 'd': vl =   +1; vr =   -1; break;  // right
			case 'b': vl = +0.5; vr =   +1; break;  // left-up
			case 'c': vl =   +1; vr = +0.5; break;  // right-up
			case 'f': vl = -0.5; vr =   -1; break;  // left-down
			case 'g': vl =   -1; vr = -0.5; break;  // right-down
			case '1': Bumper_state = 0; break;
			case '2': Bumper_state = 1; break;
			case '3': Kinect_state = 0; break;
			case '4': Kinect_state = 1; break;
			case ' ': vr = vl = 0; break;
			}
			//std::cout << "vl : " << vl << " vr : " << vr << "\n";
		    //std::cout << "state : " << *main_state << "\n";

			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}

	/* Set to point to main_state variable in Arbitrator.*/
	char * main_state;

	boost::thread _thread;
	double& vr;
	double& vl;
};

#endif /* _ACTUATOR_H_*/
