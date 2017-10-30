#ifndef _SESSION_H_
#define _SESSION_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>

class session
{
public:

	session(boost::asio::io_service& io_service) : _socket(io_service) { }

	/* Gets the Socket object associated with this session. */
	boost::asio::ip::tcp::socket& socket() {
        return _socket;
	}

	/* Starts the session. This is called in the constructor.*/
	virtual void start() = 0;

	/* Gets the data received from communication. */
	char* data() {
        return _data;
    }

protected:
	void clean() {
		memset(_data, 0, sizeof(_data));
	}

	enum { max_length = 1024 };

	boost::asio::ip::tcp::socket _socket;
	char _data[max_length];
};

#endif /* _SESSION_H */
