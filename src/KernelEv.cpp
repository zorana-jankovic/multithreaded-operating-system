/*
 * KernelEv.cpp
 *
 *  Created on: May 18, 2018
 *      Author: OS1
 */


#include "KernelEv.h"
#include "SCHEDULE.h"
#include "kernel.h"

extern volatile int lockflag;
extern volatile int lockbr;
extern volatile int prekid;
extern Data* globalData;


ID KernelEv::ident=0;
KerEvList* KernelEv::prvi=NULL;
KerEvList* KernelEv::posl=NULL;

KerEvList::KerEvList(KernelEv* e, KerEvList * sl){
	event=e;
	sled=sl;
}


KernelEv::KernelEv(IVTNo ivtNo){

	brulaza=ivtNo;
	id=ident++;
	value=0;
	flag=0;
	parent=(PCB*)PCB::running;
	myIVTEntry=IVTEntry::dohvIVTEntry(brulaza);
	if (myIVTEntry!=NULL)
		myIVTEntry->dodaj(this);
	globalData->ret=id;
	dodaj(this);
}


void KernelEv::wait(){
	//lock
	if (PCB::running!=parent){
		//unlock
		return;
	}

	if (value==0){
		PCB::running->stanje=PCB::block;
		flag=1;
		//unlock
		//dispatch();
	}
	else{
		value=0;
		//unlock
	}
}

void KernelEv::signal(){
	//lock


	if ((flag==1)&&(value==0)){
		parent->stanje=PCB::ready;
		Scheduler::put(parent);
		flag=0;
		value=0;

	}
	else
		value=1;

	//unlock
}



void KernelEv::dodaj(KernelEv* p){
	KerEvList *novi=new KerEvList(p);

	if (prvi==NULL)
		prvi=novi;
	else
		posl->sled=novi;
	posl=novi;
}

void KernelEv::ukloni(ID id){
	KerEvList *tek=prvi,*pred=NULL;

	while ((tek!=NULL)&&(tek->event->id!=id)) {
		pred=tek;
		tek=tek->sled;
	}

	if (tek==NULL)
		return;

	if (prvi==posl){
		prvi=NULL;
		posl=NULL;
		delete tek;
		return;
	}

	if (tek==prvi){
		prvi=prvi->sled;
		tek->sled=NULL;
		delete tek;
		return;
	}

	if (tek==posl){
		posl=pred;
		pred->sled=NULL;
		delete tek;
		return;
	}

	pred->sled=tek->sled;
	delete tek;
}


KernelEv* KernelEv::dohvatiEvent(ID id){
	KerEvList *tek=prvi;

		while ((tek!=NULL)&&(tek->event->id!=id))
			tek=tek->sled;

		if (tek==NULL)
			return NULL;
		else
			return tek->event;
}


KernelEv::~KernelEv(){

	if (myIVTEntry!=NULL)
		myIVTEntry->izbaci(this);

	myIVTEntry=NULL;
	//myEvent=NULL;
	parent=NULL;
	ukloni(this->id);
}
