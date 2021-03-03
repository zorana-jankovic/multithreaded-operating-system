/*
 * kerSem.h
 *
 *  Created on: May 13, 2018
 *      Author: OS1
 */

#ifndef KERSEM_H_
#define KERSEM_H_

#include "PCB.h"
#include "semaphor.h"
#include <dos.h>
#include <iostream.h>

struct semList{
	KernelSem* sem;
	semList* sled;

	semList(KernelSem* s, semList* sl=NULL);
};

class KernelSem{
private:

	static ID ident;
	ID id;
	int value;
	PCBList *prvi,*posl;
	static semList *prviSem,*poslSem;

public:
	 KernelSem(int init/*,Semaphore*/);
	 virtual ~KernelSem ();
	 virtual int wait (int toBlock);
	 virtual void signal();
	 int val () const;
	 void block();
	 void deblock();

	 static void dodaj(KernelSem*);
	 static void ukloni(ID);
	 static KernelSem* dohvatiSem(ID);
};




#endif /* KERSEM_H_ */
