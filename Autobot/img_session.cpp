/* img_session.cpp */
#include "img_session.h"
#include <session.h>

/* Public Methods */
img_session:: img_session(boost::asio::io_service& io_service, std::string * img_stream, int * img_lock) : session(io_service), img_stream(img_stream), img_lock(img_lock){
	if (!img_stream | !img_lock) {
		std::cout << "[ERROR] img_session.cpp: NullPointerExeception in Constructor\n";
	}
}

img_session::~img_session() { }

void img_session::start()
{
    std::cout << "connection from " << _socket.remote_endpoint() << "\n";
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
		//std::cout << "[INFO ] IMG READ" << std::endl;
		if (strncmp(_data, "READY", 5) == 0) {
			//std::cout << "[INFO ] READY signal received..." << std::endl;
			while (*img_lock == 1) boost::this_thread::yield();
			*img_lock = 1;
			message = "0000" + *img_stream;
			size_t len = message.length() - 4;

			//std::cout << "len = " << len << std::endl;
			message[0] = (len >> 24) & 0xFF;
			message[1] = (len >> 16) & 0xFF;
			message[2] = (len >> 8) & 0xFF;
			message[3] = len & 0xFF;
			//std::cout << "img_session.cpp: bytes[] = " << (int) message[0] << " " << (int)message[1] << " " << (int)message[2] << " " << (int)message[3] << std::endl;
			//std::cout << "img_session.cpp: message.length() = " << message.length() << std::endl;
			//std::cout << "img_session.cpp: HEAD: " << message.substr(4, 1024) << std::endl;
			//std::cout << "img_session.cpp: TAIL: " << message.substr(message.length() - 1024, message.length() - 1);

			boost::asio::async_write(_socket,
				boost::asio::buffer(message), 
				boost::bind(&img_session::handle_write, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			
			*img_lock = 0;
		}
		else {
			std::cout << "[INFO ] READY signal not received..." << std::endl;
		}
		clean();
	}
	else
	{
		delete this;
	}
}

void img_session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{

		_socket.async_read_some(boost::asio::buffer(_data, max_length),
			boost::bind(&img_session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

	}
	else
	{
		std::cout << __FUNCTION__ << ":" << error.message() << "\n";
		delete this;
	}
}
