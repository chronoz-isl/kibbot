#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <iostream>
#include <session.h>
#include <cstring>

void onMessageReceived(char * received, char * cmd_out) {
	std::cout << "received : " << *received << std::endl;
	*cmd_out = received[0];
}

#endif /* _INTERRUPTS_H_ */
