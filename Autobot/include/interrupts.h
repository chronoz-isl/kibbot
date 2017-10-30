#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <iostream>
#include <session.h>

void onMessageReceived(session* s, char * cmd_out) {
	std::cout << "received : " << s->data()[0] << std::endl;
	*cmd_out = s->data()[0];
}

#endif /* _INTERRUPTS_H_ */
