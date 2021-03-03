/*
 * PCB.h
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#ifndef PCB_H_
#define PCB_H_
#include <stdlib.h>
#include <iostream.h>
#include "thread.h"
#include "kernel.h"





#define lock lockflag=1;\
			 lockbr++;


#define unlock lockbr--;\
	if (lockbr==0) lockflag=0;\
	if ((prekid==1)&&(lockflag==0)) dispatch();

struct PCBList{
	PCB* pcb;
	PCBList* sled;
	PCBList(PCB* p1,PCBList* sl=NULL);
};

class PCB{
public:
	enum Status{ready,block,over,nova,spava};
	Status stanje;

private:
	unsigned int ss,sp;
	StackSize stackSize;
	Time timeSlice;
	Time timeToSleep;
	static PCBList *prvis,*posls;
	static PCBList *prvi,*posl;
	PCBList *prviblock,*poslblock;
	unsigned * stek;
	static ID ident;
	ID id;

	friend class KernelSem;
	friend class KerThread;
	friend class Thread;



public:

	volatile static  PCB* running;
	volatile static PCB* main;
	volatile static Thread* idle;


	void start();
	void waitToComplete();
	~PCB();

	static void sleep(Time timeToSleep);

	PCB(StackSize s1, Time t1,Thread* t);

	static void interrupt timer();
	static void inic();
	static void restore();

	void createProcess(Thread*);

	static void wrapper();

	static void dodajs(PCB*);
	static PCBList* uklonis(PCBList*);
	void dodajb(PCB*);

	static void dodaj(PCB*);
	static void ukloni(ID);
	static PCB* dohvatiPCB(ID);

	static void azuriraj_sleep();


};




#endif /* PCB_H_ */
