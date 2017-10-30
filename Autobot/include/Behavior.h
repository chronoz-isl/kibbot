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

class HitBehavior : public Behavior {
public :
	HitBehavior(int *isHit, char * cmd):hit_v(isHit), cmd(cmd) {
		if (!isHit | !cmd) {
			std::cout << "[ERROR] Behavior.h: NullPointerException in Constructor\n";
		}
	}
	char get_suggested_state() {
		if (*hit_v > 0) {
			if (*cmd == 'w' || *cmd == 'b' || *cmd == 'c') {
				return ' ';
			}
			else {
				return *cmd;
			}
			
		}
		else {
			return 'x';
		}
	}
protected :
	int* hit_v;
	char * cmd;
};

/* This class represents the User Controled Behavior.
 * The behavior has the lowest priority.
 */

class UserControl : public Behavior {
public:

	UserControl(char * robot_direction) : robot_direction(robot_direction) {
		if (!robot_direction) {
			std::cout << "[ERROR] Behavior.h: NullPointerException in Constructor\n";
		}
	}

	char get_suggested_state() {
		return *robot_direction;
	}


protected:
	char * robot_direction;
};

#endif // ! _BEHAVIOR_H_