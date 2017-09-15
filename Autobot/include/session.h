#ifndef _SESSION_H_
#define _SESSION_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>

class session
{
public:
	session(boost::asio::io_service& io_service, char * cmd_out);
	~session();

	/* Gets the Socket object associated with this session. */
	boost::asio::ip::tcp::socket& socket();

	/* Starts the session. This is called in the constructor.*/
	void start();

	/* Gets the data received from communication. */
	char* data();

private:
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);

	enum { max_length = 1024 };

	int count = 0;
	boost::asio::ip::tcp::socket _socket;
	char _data[max_length];
	char * cmd_out;
};

#endif /* _SESSION_H */
