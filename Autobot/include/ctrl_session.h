#ifndef _CTRL_SESSION_H_
#define _CTRL_SESSION_H_

#include <session.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

class ctrl_session : public session
{
public:
	ctrl_session(boost::asio::io_service& io_service, char * cmd_out);
	~ctrl_session();

	/* Starts the session. This is called in the constructor.*/
	void start();

	void set_cmd_out(char* cmd_out) {
		this->cmd_out = cmd_out;
	}

private:
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);
	char * cmd_out;
};

#endif /* _CTRL_SESSION_H */
