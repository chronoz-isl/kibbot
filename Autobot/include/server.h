#ifndef _SERVER_H_
#define _SERVER_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "cv.h"

#include <session.h>
#include <ctrl_session.h>
#include <img_session.h>

template<typename T>
class server
{
public:

	server(boost::asio::io_service& io_service, short port, char * cmd_out)
		: _io_service(io_service),
		_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		cmd_out(cmd_out)
	{
		std::cout << "running cmd_out on port " << port << std::endl;
		start_accept();
	}

	server(boost::asio::io_service& io_service, short port, std::string* img_stream, int * img_lock)
		: _io_service(io_service),
		_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		img_stream(img_stream),
		img_lock(img_lock)
	{
		std::cout << "running img on port " << port << std::endl;
		start_accept();
	}

	~server() {}

private:
	void start_accept()
	{
		session* new_session;
		if (std::is_base_of<ctrl_session, T>::value) {
			std::cout << "[DEBUG] server.h: instance of ctrl_session found" << std::endl;
			new_session = new ctrl_session(_io_service, cmd_out);
		}
		else if (std::is_base_of<img_session, T>::value) {
			std::cout << "[DEBUG] server.h: instance of img_session found" << std::endl;
			new_session = new img_session(_io_service, img_stream, img_lock);
		}
		else {
			std::cout << "[FATAL] Unsupported session type for server!" << std::endl;
		}
		_acceptor.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}

		start_accept();
	}
	session* _current_session;
	boost::asio::io_service& _io_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	

	char * cmd_out;
	std::string * img_stream;
	int * img_lock;
};




#endif // _SERVER_H_
