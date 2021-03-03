/*
 * idle.h
 *
 *  Created on: May 12, 2018
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_
#include  "thread.h"
#include "PCB.h"

class Idle:public Thread{
public:
	void run();
};



#endif /* IDLE_H_ */
