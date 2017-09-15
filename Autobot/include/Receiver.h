#ifndef _RECEIVER_H_
#define _RECEIVER_H_


#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <server.h>

/* Class which receives instructions from user. */
class Receiver {
public:
	Receiver(int port, char * cmd_out) : _port(port) , cmd_out(cmd_out) {}

	void start() {
		_thread = boost::thread(&Receiver::run, this);
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

			server s(io_service, CTRL_TCP_PORT, cmd_out);

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