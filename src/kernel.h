/*
 * kernel.h
 *
 *  Created on: May 22, 2018
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_
#include "PCB.h"
#include "event.h"

enum name{semCreate,threadCreate,threadStart,threadWaitToComplete,threadSleep,
		  threadDelete,semSignal,semWait,semVal,semDelete,eventCreate,eventWait,
		  eventDelete,DISPATCH};


struct Data{
	name ime;
	int init;
	StackSize stackSize;
	Time timeSlice;
	Thread* begin;
	ID threadID;
	Time timeToSleep;
	ID semID;
	int toBlock;
	ID eventID;
	IVTNo brUlaza;
	ID ret;
};



class KerThread : public Thread{
private:

	int radi;

public:
	volatile static KerThread* runningKernelThread;
	static void  interrupt SysCall(...);
	static void interrupt SysCallBack(...);



	void run();

};



#endif /* KERNEL_H_ */
