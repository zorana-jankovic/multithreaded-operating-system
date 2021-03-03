/*
 * KernelEv.h
 *
 *  Created on: May 18, 2018
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "event.h"
#include "thread.h"
#include "PCB.h"
#include "IVTEntry.h"

typedef unsigned char IVTNo;

class IVTEntry;
class Event;

struct KerEvList{
	KernelEv* event;
	KerEvList* sled;

	KerEvList(KernelEv* e, KerEvList* sl=NULL);
};

class KernelEv{
private:
	static ID ident;
	ID id;
	static KerEvList *prvi,*posl;
	IVTNo brulaza;
	int value;
	int flag;
	PCB* parent;
	IVTEntry* myIVTEntry;

public:
	KernelEv(IVTNo ivtNo/*, Event* par*/);
	~KernelEv();

	void wait();


	static void dodaj(KernelEv*);
	static void ukloni(ID);
	static KernelEv* dohvatiEvent(ID);


protected:
	friend class Event;
	friend class IVTEntry;
	void signal();


};




#endif /* KERNELEV_H_ */
