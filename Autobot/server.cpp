#include "server.h"

/* Public Methods */

server::server(boost::asio::io_service& io_service, short port, char * cmd_out)
	: _io_service(io_service),
	_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
	cmd_out(cmd_out)
{
	start_accept();
}

server::~server() { }

void server::stop() {
	_io_service.stop();
}

/* Private Methods */

void server::start_accept()
{
	session* new_session = new session(_io_service, cmd_out);
	_acceptor.async_accept(new_session->socket(),
		boost::bind(&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
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


