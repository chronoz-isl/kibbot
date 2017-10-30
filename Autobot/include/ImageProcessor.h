#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include <stdio.h>
#include <iostream>

#include <config.h>
#include <boost/thread.hpp>
#include "cv.h"
#include <utility.h>

/* Class which performs actuation (motor turning, led switching, ...). */
class ImageProcessor {
public :
	ImageProcessor(std::string *img_stream, int *img_lock, int *color_lock) :
		img_stream(img_stream), img_lock(img_lock), color_lock(color_lock) {
		
		if (!img_stream || !img_lock || !color_lock) {
			std::cout << "[ERROR] ImageProcessor.h: NullPointerException in Constructor\n";
		}
	}

	void process(cv::Mat &colorImg) {
		_thread = boost::thread(&ImageProcessor::run, this, colorImg);
	}

	void join() {
		_thread.join();
	}

	void interrupt() {
		_thread.interrupt();
	}

private :
	void run(cv::Mat &colorImg) {
        cv::Size size(80, 60);
		cv::Mat dst;
		while (*color_lock == 1) boost::this_thread::yield();
		*color_lock = 1;
		cv::resize(colorImg, dst, size);
		*color_lock = 0;
		reduceDepth(&dst, 0xF0);
		cv::cvtColor(dst, dst, cv::COLOR_BGR2RGB);
		//std::cout << "[INFO ] Main.cpp: generated new image..." << endl;
		while (*img_lock == 1) boost::this_thread::yield();

		*img_lock = 1;

		(*img_stream).clear();
		*img_stream = string_compress_encode(to_string(dst));
		//std::cout << "[INFO ] img_stream.length() = " << img_stream.length() << endl;
		*img_lock = 0;

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	/* Set to point to main_state variable in Arbitrator.*/
	char * main_state;

	std::string * img_stream;
	int * img_lock;
	int * color_lock;
	boost::thread _thread;
};

#endif /* _IMAGE_PROCESSOR_H_ */
