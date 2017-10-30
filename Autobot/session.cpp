#include "session.h" 
#include <interrupts.h>

/* Public Methods */
session::session(boost::asio::io_service& io_service, char * cmd_out) : _socket(io_service), cmd_out(cmd_out) { }

session::~session() { }

boost::asio::ip::tcp::socket& session::socket() { return _socket; }

void session::start()
{
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		boost::bind(&session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

char* session::data() { return _data; }

/* Private Methods */

void session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		boost::asio::async_write(_socket,
			boost::asio::buffer(_data, bytes_transferred),
			boost::bind(&session::handle_write, this,
				boost::asio::placeholders::error));
		++count;
		onMessageReceived(this, cmd_out);
	}
	else
	{
		delete this;
	}
}

void session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		_socket.async_read_some(boost::asio::buffer(_data, max_length),
			boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}
