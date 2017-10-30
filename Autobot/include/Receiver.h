#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <server.h>
#include <ctrl_session.h>
#include <utility.h>

/* Class which receives instructions from user. */
class Receiver {
public:
	Receiver(int port, char * cmd_out) : _port(port) , cmd_out(cmd_out) {
		if (!cmd_out) {
			std::cout << "[ERROR] Receiver.h: NullPointerException in Constructor\n";
		}
	}

	void start() {
		_thread = boost::thread(&Receiver::run, this);
		applyPriority(&_thread, 5);
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

			server<ctrl_session> s(io_service, _port, cmd_out);

			io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}

	boost::thread _thread;
	int _port;
	char * cmd_out;
};
#endif _RECEIVER_H_