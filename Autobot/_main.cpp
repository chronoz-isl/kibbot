/* _main.cpp */

//#define _WIN32_WINNT 0x0501

#include <cstdlib>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include <thread>
#include <chrono>
#include <time.h>

#include "RobotConnector.h"
#include "cv.h"
#include "highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>
#include <KinectConnector.h>

#include <boost/log/trivial.hpp>

#include <config.h>

#include <utility.h>
#include <Behavior.h>
#include <Arbitrator.h>
#include <Actuator.h>
#include <ImageProcessor.h>

#ifndef DISABLE_CONTROLLER
#include <Receiver.h>
#include <Viewer.h>
#endif

using namespace std;

// BEGIN TEST CODE //


// END TEST CODE //

int main(int argc, char* argv[])
{
	// Display Settings //

	std::cout << "###### SETTINGS ######\n";
#ifdef DISABLE_CONTROLLER
	std::cout << "( ) ENABLE_CONTROLLER\n";
#else
	std::cout << "(*) ENABLE_CONTROLLER\n";
#endif
#ifdef USE_ROBOT
	std::cout << "(*) USE_ROBOT\n";
#else
	std::cout << "( ) USE_ROBOT\n";
#endif
#ifdef USE_KINECT
	std::cout << "(*) USE_KINECT\n";
#else
	std::cout << "( ) USE_KINECT\n";
#endif
	std::cout << "######################\n\n";

	BOOST_LOG_TRIVIAL(info) << "Started Robot\n";

	// State Variables //

	cout << "Initializing state variables..." << endl;

	int isHitVirtualWall = 0;
	int isHitBumper = 0;
	char cmd = ' ';
	int image_lock = 0;
	int color_lock = 0;
	int bumper_count = 0;
	std::cout << "Main IsHitVirtualWall" << &isHitVirtualWall << std::endl;
	std::cout << "Main IsHitBumper" << &isHitBumper << std::endl;

	// Robot Variables //
#ifdef USE_ROBOT
	CreateData	robotData;
	RobotConnector	robot;


	if (!robot.Connect(Create_Comport))
	{
		cout << "Error : Can't connect to robot @" << Create_Comport << endl;
		return -1;
	}

#endif

#ifdef USE_KINECT
	// Connect Kinect //

	KinectConnector kin = KinectConnector();
	if (!kin.Connect()) return 1;
	std::cout << "Kinect Found!" << std::endl;

	// Kinect Variables //

	cv::Mat depthImg;
	cv::Mat colorImg;
	cv::Mat indexImg;
	cv::Mat pointImg;
#else
	cv::Mat colorImg(960, 1280, CV_8UC3, cv::Scalar(0, 100, 255));
#endif
	std::string img_stream;

	double vx = 0, vz = 0;

	// Initialize Behaviors //
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing behaviors..." << endl;

	const size_t behavior_count = 3;
	HitBehavior b0(&isHitBumper, &cmd);
	HitBehavior b1(&isHitVirtualWall, &cmd);
	UserControl b2(&cmd);
	Behavior* behaviors[behavior_count] = { &b0, &b1, &b2 };

	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing components..." << endl;

	// Initialize Components //

#ifndef DISABLE_CONTROLLER
	cout << "Initilizing rcv..." << endl;
	Receiver*   rcv = &Receiver(CTRL_TCP_PORT, &cmd);
	boost::this_thread::sleep(boost::posix_time::milliseconds(INIT_DELAY_MS));
	cout << "Initilizing vwr and imp..." << endl;
	Viewer*     vwr = &Viewer(VIEW_TCP_PORT, &img_stream, &image_lock);
	ImageProcessor* imp = &ImageProcessor(&img_stream, &image_lock, &color_lock);
	boost::this_thread::sleep(boost::posix_time::milliseconds(INIT_DELAY_MS));

#endif
	cout << "Initilizing arb..." << endl;
	Arbitrator* arb = &Arbitrator(&behaviors[0], behavior_count);
	boost::this_thread::sleep(boost::posix_time::milliseconds(INIT_DELAY_MS));
	cout << "Initilizing act..." << endl;
#ifdef USE_ROBOT
	Actuator*   act = &Actuator(vx, vz, arb, &robotData, &robot);
#else
	Actuator*   act = &Actuator(vx, vz, arb, 0, 0);
#endif
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));

	// Start Components //

#ifndef DISABLE_CONTROLLER
	cout << "Starting rcv..." << endl;
	rcv->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(START_DELAY_MS));
	cout << "Starting vwr and imp..." << endl;
	vwr->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(START_DELAY_MS));
#endif
	cout << "Starting arb..." << endl;
	arb->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(START_DELAY_MS));
	cout << "Starting act..." << endl;
	act->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(START_DELAY_MS));

#ifdef DISABLE_CONTROLLER
	cvNamedWindow("Robot");
#endif

	std::cout << "Started all components" << endl;

	// Main thread loop until received 'q' //

	while (cmd != 'q') {
		//cout << "running..." << endl;
		while (color_lock == 1) boost::this_thread::yield();
		color_lock = 1;
#ifndef USE_KINECT
		int R = (clock() * 53) % 256;
		int G = (clock() * 77) % 256;
		int B = (clock() * 19) % 256;
		colorImg = cv::Scalar(R, G, B);
		color_lock = 0;
#else
		kin.GrabData(depthImg, colorImg, indexImg, pointImg);
		color_lock = 0;
		int hit_ctr = 0;
		for (int i = 20; i < 30; i++) {
			//cout << "i = " << i << ", hit_ctr = " << hit_ctr << endl;
			for (int j = 0; j < 80; j++) {
				int depth1 = *(depthImg.ptr<int>(i, j));
				if ((depth1 < 650) && (depth1 != 0)) {
					hit_ctr++;
				}
			}
		}
		if (hit_ctr >= 5) {
			cout << "Hit!!!!" << endl;
			isHitVirtualWall = 1;
		}
		else {
			isHitVirtualWall = 0;
		}

#endif
#ifdef USE_ROBOT
		if (robotData.bumper[0] || robotData.bumper[1]) {
			bumper_count++;
		}
		else {
			bumper_count--;
			bumper_count = bumper_count < 0 ? 0 : bumper_count;
		}
		if (bumper_count >= BUMPER_THRESHOLD) {
			cout << "bumped!" << endl;
			isHitBumper = 1;
		}
		else {
			isHitBumper = 0;
		}
#endif
#ifdef DISABLE_CONTROLLER
		cmd = cvWaitKey(100);
		if (cmd == -1) {
			cmd = ' ';
		}
		else if (cmd == 27) {
			break;
		}
		cout << *(arb->main_state()) << endl;
#else
		imp->process(colorImg);
#endif
	}

	cout << "Stopping all components..." << endl;
	boost::this_thread::sleep(boost::posix_time::milliseconds(STOP_DELAY_MS));

	// Stop all components //

	act->interrupt();
	arb->interrupt();
#ifndef DISABLE_CONTROLLER
	rcv->interrupt();
	vwr->interrupt();
#endif

	// Wait for all components to stop //

	arb->join();
	act->join();

	cout << "Finished Execution." << endl;
	return 0;
}
