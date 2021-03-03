/*
 * IVTEntry.h
 *
 *  Created on: May 18, 2018
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_
#include <iostream.h>
#include "event.h"
#include "KernelEv.h"

class KernelEv;
typedef unsigned char IVTNo;

/*#define PREPAREENTRY(num,f)\
void interrupt inter##num(...);\
IVTEntry newEntry##num(num,inter##num);\
void interrupt inter##num(...){\
	newEntry##num.signal();\
	if (f)\
		newEntry##num.callOld();\
}*/




typedef void interrupt (*pInterrupt)(...);

struct eventList{
	KernelEv* dogadjaj;
	eventList* sled;

	eventList(KernelEv* dog, eventList* sl=NULL);
};


class IVTEntry{
private:
	static IVTEntry* ulazi[256];
	eventList *prviEvent,*poslEvent;
	pInterrupt old;
	IVTNo numEntry;


public:

	IVTEntry(IVTNo brUlaza,pInterrupt nova);
	~IVTEntry();

	static IVTEntry* dohvIVTEntry(IVTNo n);
	void dodaj(KernelEv*);
	void izbaci(KernelEv*);
	void signal();


	void callOld();

};

#define PREPAREENTRY(num,f)\
void interrupt inter##num(...);\
IVTEntry newEntry##num(num,inter##num);\
void interrupt inter##num(...){\
	newEntry##num.signal();\
	if (f)\
		newEntry##num.callOld();\
}


#endif /* IVTENTRY_H_ */
