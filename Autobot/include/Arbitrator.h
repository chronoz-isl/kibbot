#ifndef  _ARBITRATOR_H_
#define  _ARBITRATOR_H_
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <boost/thread.hpp>

#include <Behavior.h>

/* Class which performs arbitration (controlling which behavior is used). */
class Arbitrator {
public:

	// Constructor //
	
	Arbitrator(Behavior** behaviors, size_t behavior_count) : 
		_behaviors(behaviors), 
		_behavior_count(behavior_count) { }

	// Thread Methods //

	void start() {
		_thread = boost::thread(&Arbitrator::run, this);
	}

	void join() {
		_thread.join();
	}

	void interrupt() {
		_thread.interrupt();
	}

	// Getter Methods //

	Behavior** behaviors() { return _behaviors; }
	char * main_state() { return &_main_state; }
	size_t behavior_count() { return _behavior_count; }


private:
	void run() {
		while (true)
		{
			for (size_t i = 0; i < _behavior_count; i++) {

				// If this behavior returned a don't care (x), 
				// continue to the next behavior. Else, do what the behavior wants.
				if (_behaviors[i]->get_suggested_state() == 'x') {
					continue;
				}
				else {
					_main_state = _behaviors[i]->get_suggested_state();
				}
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}

	// Fields //

	/* Array of pointers to behaviors.*/
	Behavior** _behaviors;
	char _main_state;
	size_t _behavior_count;
	boost::thread _thread;
};

#endif // ! _ARBITRATOR_H_;
