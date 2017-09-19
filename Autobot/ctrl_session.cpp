/* ctrl_session.cpp */
#include "ctrl_session.h" 
#include <session.h>
#include <interrupts.h>

/* Public Methods */
ctrl_session::ctrl_session(boost::asio::io_service& io_service, char * cmd_out) : session(io_service) {
	this->cmd_out = cmd_out;
}

ctrl_session::~ctrl_session() { }

void ctrl_session::start()
{
	std::cout << "connection from " << _socket.remote_endpoint() << "\n";
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		boost::bind(&ctrl_session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

/* Private Methods */

void ctrl_session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		boost::asio::async_write(_socket,
			boost::asio::buffer(_data, bytes_transferred),
			boost::bind(&ctrl_session::handle_write, this,
				boost::asio::placeholders::error));
	}
	else
	{
		delete this;
	}
}

void ctrl_session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		onMessageReceived(_data, cmd_out);
		_socket.async_read_some(boost::asio::buffer(_data, max_length),
			boost::bind(&ctrl_session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}
