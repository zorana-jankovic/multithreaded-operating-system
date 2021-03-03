/*
 * kerSem.cpp
 *
 *  Created on: May 13, 2018
 *      Author: OS1
 */

#include "kerSem.h"
#include "SCHEDULE.H"
#include "kernel.h"

extern volatile int lockflag;
extern volatile int lockbr;
extern volatile int prekid;
extern Data* globalData;


ID KernelSem::ident=0;
semList* KernelSem::prviSem=NULL;
semList* KernelSem::poslSem=NULL;

semList::semList(KernelSem* s, semList* sl){
	sem=s;
	sled=sl;
}



KernelSem::KernelSem(int init/*, Semaphore* par*/){

	value=init;
	id=ident++;
	prvi=posl=NULL;
	globalData->ret=id;
	dodaj(this);
}

int KernelSem::wait(int toBlock){
	//lock
	if (toBlock!=0){
		if(--value<0){
			block();
			return 1;
		}
		//unlock
		return 0;
	}

	else{
		if (value<=0){
			//unlock
			return -1;
		}

		value--;
		//unlock
		return 0;
	}
}

void KernelSem::signal(){
	//lock
	if (value++<0)
		deblock();
	//unlock
}


int KernelSem::val()const{
	return value;
}


void KernelSem::block(){
	PCB::running->stanje=PCB::block;
	PCBList *novi=new PCBList((PCB*)PCB::running);

	if(prvi==NULL)
		prvi=novi;
	else
		posl->sled=novi;
	posl=novi;
	//unlock
	//dispatch();
}


void KernelSem::deblock(){
	PCBList * tek=prvi;

	if(prvi==posl)
		prvi=posl=NULL;
	else
		prvi=prvi->sled;

	tek->pcb->stanje=PCB::ready;
	Scheduler::put(tek->pcb);

	delete tek;

}


void KernelSem::dodaj(KernelSem* k){
	semList *novi=new semList(k);

	if (prviSem==NULL)
		prviSem=novi;
	else
		poslSem->sled=novi;
	poslSem=novi;
}

void KernelSem::ukloni(ID id){
	semList *tek=prviSem,*pred=NULL;

	while ((tek!=NULL)&&(tek->sem->id!=id)) {
		pred=tek;
		tek=tek->sled;
	}

	if (tek==NULL)
		return;

	if (prviSem==poslSem){
		prviSem=NULL;
		poslSem=NULL;
		delete tek;
		return;
	}

	if (tek==prviSem){
		prviSem=prviSem->sled;
		tek->sled=NULL;
		delete tek;
		return;
	}

	if (tek==poslSem){
		poslSem=pred;
		pred->sled=NULL;
		delete tek;
		return;
	}

	pred->sled=tek->sled;
	delete tek;
}

KernelSem* KernelSem::dohvatiSem(ID id){
	semList *tek=prviSem;

		while ((tek!=NULL)&&(tek->sem->id!=id))
			tek=tek->sled;

		if (tek==NULL)
			return NULL;
		else
			return tek->sem;
}


KernelSem::~KernelSem(){

	PCBList* tek=prvi,*stari;

	while(tek!=NULL){
		tek->pcb->stanje=PCB::ready;
		Scheduler::put(tek->pcb);
		stari=tek;
		tek=tek->sled;
		delete stari;
	}

	prvi=NULL;
	posl=NULL;
	ukloni(this->id);
	//mySem=NULL;
}
