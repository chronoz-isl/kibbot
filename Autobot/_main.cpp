#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <KinectConnector.h>
#include "RobotConnector.h"
#include "cv.h"
#include "highgui.h"
#include <RecieverThread.h>
#include <Behavior.h>
#include <Arbitrator.h>

#include <thread>
#include <chrono>


#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"


#define Create_Comport "COM3"

bool isRecord = false;
double vx = 0;
double vz = 0;


using namespace std;
using namespace cv;

int main() {

	RecieverThread rt(vx, vz);
	thread t(rt);
	t.detach();
	
	CreateData	robotData;
	RobotConnector	robot;
	ofstream	record;
	record.open("../data/robot.txt");

	Behavior* bs[3];
	bs[0] = &(HitBumper(&Bumper_state));
	bs[1] = &(HitVirtualWall(&Kinect_state));
	bs[2] = &(UserControl());
	Arbitrator arbt(bs);
	arbt.start();


	/*while (true) {

		cout << "vx : " << vx << " vz : " << vz << endl;

		std::this_thread::sleep_for(std::chrono::seconds(2));

	}*/

	/*if (!robot.Connect(Create_Comport))
	{
		cout << "Error : Can't connect to robot @" << Create_Comport << endl;
		return -1;
	}

	robot.DriveDirect(0, 0);
	cvNamedWindow("Robot");


	KinectConnector kin = KinectConnector();


	if (!kin.Connect()) return 1;
	

	while (true) {
		Mat depthImg;
		Mat colorImg;
		Mat indexImg;
		Mat pointImg;

		kin.GrabData(depthImg, colorImg, indexImg, pointImg);
		imshow("depthImg", depthImg);
		//imshow("colorImg", colorImg);
		imshow("indexImg", indexImg);
		imshow("pointImg", pointImg);
		double minVal;
		double maxVal;
		Point minLoc;
		Point maxLoc;

		minMaxLoc(depthImg, &minVal, &maxVal, &minLoc, &maxLoc);
		cout << minVal << " " << maxVal << endl;
		if (depthImg.isContinuous()) {
			cout << true << endl;
		}
		NuiCameraElevationSetAngle(0);
		char c = waitKey(1);
	}*/
	return 0;
}