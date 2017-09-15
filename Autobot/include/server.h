#ifndef _SERVER_H_
#define _SERVER_H_

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <session.h>

class server
{
public:

	server(boost::asio::io_service& io_service, short port, char * cmd_out);
	~server();
	void stop();

private:
	void start_accept();

	void handle_accept(session* new_session,
		const boost::system::error_code& error);

	session* _current_session;
	boost::asio::io_service& _io_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	

	char * cmd_out;
};

#endif // _SERVER_H_
