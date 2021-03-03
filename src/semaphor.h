/*
 * semaphor.h
 *
 *  Created on: May 13, 2018
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_
#include "thread.h"
class KernelSem;

class Semaphore {
public:

 Semaphore (int init=1);
 virtual ~Semaphore ();
 virtual int wait (int toBlock);
 virtual void signal();
 int val () const; // Returns the current value of the semaphore

private:

 ID myImpl;
};



#endif /* SEMAPHOR_H_ */
