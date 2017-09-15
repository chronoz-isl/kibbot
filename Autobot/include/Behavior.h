#pragma once
#ifndef  _BEHAVIOR_H_
#define  _BEHAVIOR_H_

#include <stdio.h>
#include <iostream>

using namespace std;

class Behavior {
public:
	void set_suggested_state(char state) {
		suggested_state = state;
	}
	virtual char get_suggested_state()=0;
protected:
	char suggested_state;
};

class HitVirtualWall : public Behavior {
public :
	HitVirtualWall(int *isHit):hit_v(isHit) {
	}
	char get_suggested_state(){
		if (*hit_v) {
			return ' ';
				 
		}
		else {
			return 'x';
		}
	}
protected :
	int* hit_v;
};

class HitBumper : public Behavior {
public:
	HitBumper(int *isHit):hit_b(isHit) {
	}
	char get_suggested_state() {
		return ' ';
	}
protected:
	int* hit_b;
};
class UserControl : public Behavior {
public:

	void set_suggested_state(char ss) {
		robot_direction = ss;
	}

	char get_suggested_state() {
		return robot_direction;
	}


protected:
	char robot_direction = ' ';
};




#endif // ! _BEHAVIOR_H_

