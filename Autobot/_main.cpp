/* _main.cpp */
#define _WIN32_WINNT 0x0501
#define Create_Comport "COM3"

/* Remote controller TCP network port. */
#define CTRL_TCP_PORT 80

/* Remote Viewer TCP network port. */
#define VIEW_TCP_PORT 81

/* Uncomment DISABLE_CONTROLLER to disable remote controller
 * and use local OpenCV window for controlling.
 */
//#define DISABLE_CONTROLLER

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

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>

#include <utility.h>
#include <Behavior.h>
#include <Arbitrator.h>
#include <Actuator.h>

#ifndef DISABLE_CONTROLLER
#include <Receiver.h>
#include <Viewer.h>
#endif

using namespace std;

// BEGIN TEST CODE //


#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace io = boost::asio;
namespace ip = io::ip;
using ip::tcp;

/*
void start_accept(io::io_service& svc, tcp::acceptor& acc) {
	// per-connection lifetimes:
	auto sock = boost::make_shared<tcp::socket>(svc);

	acc.async_accept(*sock, [sock, &svc, &acc](boost::system::error_code ec) {
		if (!ec)
		{
			std::cout << "connection from " << sock->remote_endpoint() << "\n";
			// copy source file to buffer data
			auto data = boost::make_shared<io::streambuf>();
			std::ostream(data.get()) << string_compress_encode(to_string(a));

			// now write the whole story
			io::async_write(*sock, *data, [sock, data](boost::system::error_code ec, size_t transferred) {});

			cout << "FIN";
			// accept new connections too
			start_accept(svc, acc);
		}
	});
}
*/

// END TEST CODE //

int main(int argc, char* argv[])
{
	//cout << "Transmitting solid color: " << a.row(0).col(0) << endl;
	//cout << "rows = " << a.rows << ", cols = " << a.cols << endl;
	//uchar* hot = a.ptr(10, 10);
	//for (int i = 0; i < 9; i++) {
	//	*hot++ = 33;
	//}
	//cout << (int) *a.data << endl;
	//cout << (int) *(a.data + 1280 * 3) << endl;
	//cout << a.row(1).col(0) << endl;
	//cout << a.col(1).row(0) << endl;

	//io::io_service svc;
	//tcp::acceptor acc(svc, tcp::endpoint(ip::address(), 81));
	//start_accept(svc, acc);
	//svc.run();
	//return 10;
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	cout << "Initializing state variables..." << endl;
	// State Variables //
	int isHitVirtualWall = 0;
	int isHitBumper = 0;
	char cmd = ' ';
	int image_lock = 0;
	cv::Mat a(960, 1280, CV_8UC3, cv::Scalar(0, 100, 255));
	std::string img_stream;

	double vx = 0, vz = 0;

	// Initialize Behaviors //
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing behaviors..." << endl;


	const size_t behavior_count = 3;
	Behavior* behaviors[behavior_count];
	behaviors[0] = &HitBumper(&isHitBumper);
	behaviors[1] = &HitVirtualWall(&isHitVirtualWall);
	behaviors[2] = &UserControl(&cmd);

	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing components..." << endl;

	// Initialize Components //

#ifndef DISABLE_CONTROLLER
	cout << "Initilizing rcv..." << endl;
	Receiver*   rcv = &Receiver(CTRL_TCP_PORT, &cmd);
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing vwr..." << endl;
	Viewer*     vwr = &Viewer(VIEW_TCP_PORT, &img_stream, &image_lock);
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));

#endif
	cout << "Initilizing arb..." << endl;
	Arbitrator* arb = &Arbitrator(behaviors, behavior_count);
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Initilizing act..." << endl;
	Actuator*   act = &Actuator(vx, vz, arb);
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));

	// Start Components //

#ifndef DISABLE_CONTROLLER
	cout << "Starting rcv..." << endl;
	rcv->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Starting vwr..." << endl;
	vwr->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
#endif
	cout << "Starting arb..." << endl;
	arb->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));
	cout << "Starting act..." << endl;
	act->start();
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));

#ifdef DISABLE_CONTROLLER
	cvNamedWindow("Robot");
#endif

	cout << "Started all components" << endl;
	// Main thread loop until received 'q' //

	while (cmd != 'q') {
		int R = (clock() * 53) % 256;
		int G = (clock() * 77) % 256;
		int B = (clock() * 19) % 256;
		a = cv::Scalar(R, G, B);
		cv::Size size(640, 480);
		cv::Mat dst;
		cv::resize(a, dst, size);
		cout << "[INFO ] Main.cpp: generated new image..." << endl;
		while (image_lock == 1) boost::this_thread::yield();
		image_lock = 1;
		img_stream = string_compress_encode(to_string(dst));
		image_lock = 0;
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
		boost::this_thread::yield();
#endif
	}

	cout << "Stopping all components..." << endl;
	boost::this_thread::sleep(boost::posix_time::milliseconds(200));

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
	// [kris] rcv won't join because unable to stop io_service in rcv.

	cout << "Finished Execution." << endl;
	return 0;
}
