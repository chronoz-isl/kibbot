#ifndef _IMG_SESSION_H_
#define _IMG_SESSION_H_

#include <session.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "cv.h"
#include <iostream>
#include <utility.h>
#include <boost/thread.hpp>

class img_session : public session
{
public:
	img_session(boost::asio::io_service& io_service, std::string * img_stream, int*  img_lock);
	~img_session();

	/* Starts the session. This is called in the constructor.*/
	void start();

private:
	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);

	std::string * img_stream;
	int * img_lock;
};

#endif /* _IMG_SESSION_H */
