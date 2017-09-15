#ifndef _RECIEVERTHREAD_H_
#define _RECIEVERTHREAD_H_

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>



class RecieverThread {


public :
	RecieverThread(double &vxi, double &vzi) : vxin(vxi),vzin(vzi) {}

	void operator () () {
		cvNamedWindow("Robot");
		while (true)
		{
			char c = cvWaitKey(1000000);
			if (c == 27) break;
			vxin = 0;
			vzin = 0;
			switch (c)
			{
			case 'w': vxin = +1;  break;
			case 's': vxin = -1; break;
			case 'a': vzin = +1; break;
			case 'd': vzin = -1; break;
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