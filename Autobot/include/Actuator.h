#ifndef _ACTUATOR_H_
#define _ACTUATOR_H_

#include <stdio.h>
#include <iostream>

#include <config.h>
#include <boost/thread.hpp>
#include <RobotConnector.h>
#include <CreateData.h>
#include <Arbitrator.h>

/* Class which performs actuation (motor turning, led switching, ...). */
class Actuator {
public :
	Actuator(double &vxi, double &vzi, Arbitrator *arb, CreateData* robotData, RobotConnector* robot) : 
		vr(vxi),vl(vzi), main_state(arb->main_state()), robotData(robotData), robot(robot) {
		if (!arb) {
			std::cout << "[ERROR] Actuator.h: NullPointerException in Constructor\n";
		}
	}

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
			// Target Velocity //
			double tvl, tvr;

			switch (*main_state)
			{
			case 'w': tvl =   +1; tvr =   +1; break;  // up
			case 's': tvl =   -1; tvr =   -1; break;  // down
			case 'a': tvl =   -1; tvr =   +1; break;  // left
			case 'd': tvl =   +1; tvr =   -1; break;  // right
			case 'b': tvl = +0.5; tvr =   +1; break;  // left-up
			case 'c': tvl =   +1; tvr = +0.5; break;  // right-up
			case 'f': tvl = -0.5; tvr =   -1; break;  // left-down
			case 'g': tvl =   -1; tvr = -0.5; break;  // right-down
			case 'o': tvl =   vl; tvr =   vr; angle++; angle = (angle > +23 ? +23 : angle); break; // heads up
			case 'l': tvl =   vl; tvr =   vr; angle--; angle = (angle < -10 ? -10 : angle); break; // heads down
			case 'p': tvl =   vl; tvr =   vr; angle = 0; break; // heads zero
			case '1': tvl =   vl; tvr =   vr; led[0] = (++led[0] % 2); break;  // toggle led0
			case '2': tvl =   vl; tvr =   vr; led[1] = (++led[1] % 2); break;  // toggle led1
			case ' ': tvr = tvl = 0; break;
			}
			//std::cout << "tvl : " << tvl << " tvr : " << tvr << "\n";
#ifdef USE_KINECT
			if (*main_state == 'o' || *main_state == 'l' || *main_state == 'p') {
				NuiCameraElevationSetAngle(angle);
			}
#endif
		    //std::cout << "state : " << *main_state << "\n";

			// Calculate Real Velocity from Target velocity (Will approach target logarithmically) //

			vl += (tvl - vl) / 2;
			vr += (tvr - vr) / 2;

#ifdef USE_ROBOT
			int velL = (int)(vl*Create_MaxVel);
			int velR = (int)(vr*Create_MaxVel);

			//int color = (abs(velL) + abs(velR)) / 4;
			//color = (color < 0) ? 0 : (color > 255) ? 255 : color;

			int color = 255;
			int inten = 255;

			//cout << color << " " << inten << " " << robotData.cliffSignal[1] << " " << robotData.cliffSignal[2] << endl;

			robot->LEDs(led[0] > 0, led[1] > 0, color, inten);

			if (!robot->DriveDirect(velL, velR))
				cout << "SetControl Fail" << endl;

			robot->ReadData(*robotData);
#endif


			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}

	/* Set to point to main_state variable in Arbitrator.*/
	char * main_state;

	boost::thread _thread;
	double& vr;
	double& vl;
	int angle = 0;
	int led[2] = { 0 };
	
	RobotConnector * robot;
	CreateData * robotData;
};

#endif /* _ACTUATOR_H_*/
