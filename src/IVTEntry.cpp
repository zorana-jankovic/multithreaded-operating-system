/*
 * IVTEntry.cpp
 *
 *  Created on: May 18, 2018
 *      Author: OS1
 */

#include <iostream.h>
#include <dos.h>
#include "IVTEntry.h"
#include "KernelEv.h"

extern volatile int lockflag;
extern volatile int lockbr;
extern volatile int prekid;

IVTEntry* IVTEntry::ulazi[256]={NULL};


eventList::eventList(KernelEv* dog,eventList* sl){
	dogadjaj=dog;
	sled=sl;
}

IVTEntry::IVTEntry(IVTNo brUlaza, pInterrupt nova){
#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif

	numEntry=brUlaza;

#ifndef BCC_BLOCK_IGNORE
	old=getvect(numEntry);
	setvect(numEntry,nova);
#endif
	ulazi[brUlaza]=this;
	prviEvent=NULL;
	poslEvent=NULL;

#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif
}


void IVTEntry::callOld(){
	old();
#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif
}

IVTEntry* IVTEntry::dohvIVTEntry(IVTNo n){
	return ulazi[n];
}


void IVTEntry::dodaj(KernelEv* e){
	lock
	eventList* novi=new eventList(e);

	if (prviEvent==NULL)
		prviEvent=novi;
	else
		poslEvent->sled=novi;
	poslEvent=novi;
	unlock
}

void IVTEntry::izbaci(KernelEv* e){
	lock
	eventList* tek=prviEvent,*pred=tek;

	while ((e!=tek->dogadjaj)&&(tek!=NULL)){
		pred=tek;
		tek=tek->sled;
	}

	if (tek==NULL){
		unlock
		return;
	}

	if (prviEvent==poslEvent){
		prviEvent=NULL;
		poslEvent=NULL;
		delete tek;
		unlock
		return;
	}

	if (prviEvent==tek){
		prviEvent=tek->sled;
		tek->sled=NULL;
		delete tek;
		unlock
		return;
	}

	if (poslEvent==tek){
		poslEvent=pred;
		pred->sled=NULL;
		delete tek;
		unlock
		return;
	}

	pred->sled=tek->sled;
	tek->sled=NULL;
	delete tek;
	unlock

}


void IVTEntry::signal(){
	eventList* tek=prviEvent;

	while(tek!=NULL){
		tek->dogadjaj->signal();
		tek=tek->sled;
	}
}


IVTEntry::~IVTEntry(){
#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif

	eventList* tek=prviEvent,*stari=NULL;

	while(tek!=NULL){
		stari=tek;
		tek=tek->sled;
		delete tek;
	}
	prviEvent=NULL;
	poslEvent=NULL;

	ulazi[numEntry]=NULL;

#ifndef BCC_BLOCK_IGNORE
	setvect(numEntry,old);
#endif

#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif
}
