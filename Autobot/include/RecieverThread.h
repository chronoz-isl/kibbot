#ifndef _RECIEVERTHREAD_H_
#define _RECIEVERTHREAD_H_

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

int Bumper_state = 0;
int Kinect_state = 0;

class RecieverThread {


public :
	RecieverThread(double &vxi, double &vzi) : vxin(vxi),vzin(vzi) {}

	void operator () () {
		cvNamedWindow("Robot");
		while (true)
		{

			char c = cvWaitKey(1000000);
			if (c == 27) break;
			switch (c)
			{
			case 'w': vxin = +1;  break;
			case 's': vxin = -1; break;
			case 'a': vzin = +1; break;
			case 'd': vzin = -1; break;
			case '1': Bumper_state = 0; break;
			case '2': Bumper_state = 1; break;
			case '3': Kinect_state = 0; break;
			case '4': Kinect_state = 1; break;
			case ' ': vxin = vzin = 0; break;
			}
			std::cout << "vx : " << vxin << " vz : " << vzin << "\n";
		    std::cout << c << "\n";
		}
	}
private :
	double& vxin;
	double& vzin;
};








#endif