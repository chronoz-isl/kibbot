/* img_session.cpp */
#include "img_session.h"
#include <session.h>

/* Public Methods */
img_session:: img_session(boost::asio::io_service& io_service, std::string * img_stream, int * img_lock) : session(io_service), img_stream(img_stream), img_lock(img_lock){
}

img_session::~img_session() { }

void img_session::start()
{
    std::cout << "connection from " << _socket.remote_endpoint() << "\n";
	std::cout << "img_session waiting for READY signal..." << std::endl;
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		boost::bind(&img_session::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

/* Private Methods */

void img_session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		char otherString[6];
		std::cout << "[INFO ] IMG READ" << std::endl;
		if (strncmp(_data, "READY", 5) == 0) {
			std::cout << "[INFO ] READY signal received..." << std::endl;
			while (*img_lock == 1) boost::this_thread::yield();
			*img_lock = 1;
			std::string message = *img_stream;
			unsigned int len = message.length();
			*img_lock = 0;

			std::cout << "len = " << len << std::endl;
			unsigned char bytes[4];

			bytes[0] = (len >> 24) & 0xFF;
			bytes[1] = (len >> 16) & 0xFF;
			bytes[2] = (len >> 8) & 0xFF;
			bytes[3] = len & 0xFF;
			message = std::string((char* ) bytes, 4) + message;

			boost::asio::async_write(_socket,
				boost::asio::buffer(message.c_str(), message.length()),
				boost::bind(&img_session::handle_write, this,
					boost::asio::placeholders::error));
		}
		else {
			std::cout << "[INFO ] READY signal not received..." << std::endl;
			boost::asio::async_write(_socket,
				boost::asio::buffer("", 1),
				boost::bind(&img_session::handle_write, this,
					boost::asio::placeholders::error));
		}
		clean();
	}
	else
	{
		delete this;
	}
}

void img_session::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{

		std::cout << "img_session waiting for READY signal..." << std::endl;
		_socket.async_read_some(boost::asio::buffer(_data, max_length),
			boost::bind(&img_session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}
