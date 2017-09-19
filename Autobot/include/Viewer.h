#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "cv.h"

#include <server.h>
#include <img_session.h>

/* Class which sends robot viewport to user. */
class Viewer {
public:
	Viewer(int port, std::string * img_stream, int * image_lock) : _port(port) , img_stream(img_stream), img_lock(image_lock) {}

	void start() {
		_thread = boost::thread(&Viewer::run, this);
	}

	void join() {
		_thread.join();
	}

	void interrupt() {
		_thread.interrupt();
	}

private:
	void run() {
		try
		{
			boost::asio::io_service io_service;

			server<img_session> s(io_service, _port, img_stream, img_lock);

			io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}

	boost::thread _thread;
	int _port;
	std::string * img_stream;
	int * img_lock;
};
#endif /* _VIEWER_H_ */
