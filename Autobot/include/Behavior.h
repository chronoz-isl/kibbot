#ifndef  _BEHAVIOR_H_
#define  _BEHAVIOR_H_

#include <stdio.h>
#include <iostream>

/* Interface representing a Behavior to be used in the Arbitrator. */
class Behavior {
public:

   /* Base Abstract method. Called by Arbitrator to get suggestion for 
	  setting the next state. 'x' means don't care and arbitrator will
	  NOT consider a state suggestion from this Behavior.
    */
	virtual char get_suggested_state() = 0;
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

/* This class represents the User Controled Behavior.
 * The behavior has the lowest priority.
 */

class UserControl : public Behavior {
public:

	UserControl(char * robot_direction) : robot_direction(robot_direction) {
		*robot_direction = ' ';
	}

	char get_suggested_state() {
		return *robot_direction;
	}


protected:
	char * robot_direction;
};

#endif // ! _BEHAVIOR_H_
