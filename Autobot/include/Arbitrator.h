#pragma once
#ifndef  _ARBITRATOR_H_
#define  _ARBITRATOR_H_
#include <stdio.h>
#include <iostream>
#include <Behavior.h>

#define behaviors_count 3
class Arbitrator {
protected:
	Behavior** behaviors;
	char main_state;

public:
	Arbitrator(Behavior* behaviors[] ){
		this->behaviors = behaviors;
	}
	void start() {
		while (true)
		{
			for (int i = 0; i < behaviors_count; i++) {
				//std::cout << behaviors[i]->get_suggested_state() << std::endl;
				if (behaviors[i]->get_suggested_state() == 'x') {
					continue;
				}
				else {
					main_state = behaviors[i]->get_suggested_state();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}

	}
};

#endif // ! _ARBITRATOR_H_;

