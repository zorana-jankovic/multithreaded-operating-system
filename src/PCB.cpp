/*
 * PCB.cpp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#include "PCB.h"
#include "SCHEDULE.H"
#include "idle.h"
#include "kernel.h"
#include <iostream.h>
#include <dos.h>

void tick();

extern Data *globalData;



volatile unsigned int a;
volatile unsigned int b;


volatile unsigned int promena_konteksta=0;
volatile Time brojac=20;//mozda nije dobra vrednost
volatile unsigned int tsp,tss;
volatile unsigned int oldTimerOFF, oldTimerSEG;
volatile unsigned int MAXSTEK=65536;
volatile int lockflag=1;
volatile int lockbr=0;
volatile int prekid=0;



PCBList::PCBList(PCB* p1, PCBList* sl){
	pcb=p1;
	sled=sl;
}


PCBList* PCB::prvis=NULL;
PCBList* PCB::posls=NULL;
PCBList* PCB::prvi=NULL;
PCBList* PCB::posl=NULL;
volatile PCB* PCB::running=NULL;
volatile PCB* PCB::main=NULL;
volatile Thread* PCB::idle=NULL;
ID PCB::ident=0;

PCB::PCB(StackSize s1, Time t1,Thread* t){
	if (s1>MAXSTEK)
		stackSize=MAXSTEK;
	stackSize=s1;
	timeSlice=t1;
	id=ident++;
	stanje=PCB::nova;
	timeToSleep=0;
	prviblock=poslblock=NULL;
	if (t!=NULL){
		createProcess(t);
		globalData->ret=id;
	}
	dodaj(this);
}



void PCB::createProcess(Thread* begin){


#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif

	stek=new unsigned[stackSize=stackSize/2];

#ifndef BCC_BLOCK_IGNORE
	asm sti;
	stek[stackSize-1]=FP_SEG(begin);
	stek[stackSize-2]=FP_OFF(begin);
#endif

	stek[stackSize-5]=0x200;

#ifndef BCC_BLOCK_IGNORE
	stek[stackSize-6]=FP_SEG(Thread::wrapper);
	stek[stackSize-7]=FP_OFF(Thread::wrapper);

	this->sp=FP_OFF(stek+stackSize-16);
	this->ss=FP_SEG(stek+stackSize-16);
#endif
}

void PCB::start(){
	//lock
	stanje=PCB::ready;
	Scheduler::put(this);
	//unlock
}

void PCB::azuriraj_sleep(){
	if (prvis!=NULL){
			PCBList* tek=prvis,*pred=NULL,*stari;

			while(tek!=NULL){
				tek->pcb->timeToSleep--;
				if (tek->pcb->timeToSleep==0){
					tek=uklonis(pred);
				}

				else{
					pred=tek;
					tek=tek->sled;
				}

			}
		}

}

void interrupt PCB::timer(){
#ifndef BCC_BLOCK_IGNORE
		asm int 60h;
#endif
		if (brojac>0)
			brojac--;
		tick();
#ifndef BCC_BLOCK_IGNORE
		asm cli;
#endif

		azuriraj_sleep();






	if (((brojac==0)&&(running->timeSlice!=0))){
		if (lockflag==0){

#ifndef BCC_BLOCK_IGNORE
			asm{
				mov tsp,sp
				mov tss,ss
			}
#endif

			running->sp=tsp;
			running->ss=tss;

			if ((running->stanje==ready)&&(running->id!=idle->myPCB))
				Scheduler::put((PCB*)running);

			running=Scheduler::get();



			if(running==NULL)
				running=dohvatiPCB(idle->myPCB);

			brojac=running->timeSlice;
			tsp=running->sp;
			tss=running->ss;

#ifndef BCC_BLOCK_IGNORE
			asm{
				mov sp, tsp
				mov ss, tss
			}
#endif


			promena_konteksta=0;
			prekid=0;
		}

		else
			prekid=1; // GDE SE KORISTI?
	}

}

// KAD SE ZOVE?
void PCB::inic(){
#ifndef BCC_BLOCK_IGNORE
	asm cli;
	a=FP_SEG(timer);
	b=FP_OFF(timer);

	asm{
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax,word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax,word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov ax, a
		mov word ptr es:0022h, ax
		mov ax, b
		mov word ptr es:0020h, ax

		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es

		sti
	}

	setvect(0x61,KerThread::SysCall);
	setvect(0x63,KerThread::SysCallBack);
#endif
//lock


	main=new PCB(0,1,NULL);
	main->stanje=PCB::ready;
	running=main;
	KerThread::runningKernelThread=new KerThread();

	idle=new Idle();


//unlock
}

void PCB::restore(){
#ifndef BCC_BLOCK_IGNORE
	asm{
		cli
		push es
		push ax

		mov ax, 0
		mov es, ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
#endif

	delete idle;
	delete KerThread::runningKernelThread;
	delete main;
}

void PCB::sleep(Time t){
	//lock
	running->stanje=PCB::spava;
	running->timeToSleep=t;
	dodajs((PCB*)running);
	//unlock
	//dispatch();
}

void PCB::dodajs(PCB* p){
	//lock
	PCBList* novi=new PCBList(p);

	if (prvis==NULL)
		prvis=novi;
	else
		posls->sled=novi;
	posls=novi;
	//unlock
}


PCBList* PCB::uklonis(PCBList* pret){
	//lock
	PCBList* tek=pret->sled,*pom,*stari;


	if(prvis==posls){
		tek=prvis;
		tek->pcb->stanje=PCB::ready;
		Scheduler::put(tek->pcb);
		prvis=NULL;
		posls=NULL;
		delete tek;
		//unlock
		return NULL;
	}

	if (pret==NULL){
		tek=prvis;
		prvis=tek->sled;
	}

	else{
		pret->sled=tek->sled;
	}

	if (tek==posls)
		posls=pret;

	tek->pcb->stanje=PCB::ready;
	Scheduler::put(tek->pcb);

	stari=tek;
	tek=tek->sled;
	delete stari;
	//unlock
	return tek;
}

void PCB::dodajb(PCB* r){
	//lock
	PCBList* novi=new PCBList(r);

	if (prviblock==NULL)
		prviblock=novi;
	else
		poslblock->sled=novi;
	poslblock=novi;
	//unlock
}


void PCB::waitToComplete(){
	//lock
	if(this->stanje==PCB::over){
		//unlock
		return;
	}
	if ((this->id==main->id)||(this->id==idle->myPCB)||(this->id==KerThread::runningKernelThread->myPCB)){
		//unlock
		return;
	}
	running->stanje=PCB::block;
	dodajb((PCB*)running);
	//unlock//mora pre dispatch(), inace nista nece ni da se dogodi!!!
	//dispatch();
}


void PCB::wrapper(){
	/*running->myThread->run();

	lock
	running->stanje=PCB::over;
	PCBList* tek=running->prviblock,*stari;
	while(tek!=NULL){
		stari=tek;
		tek=tek->sled;
		stari->pcb->stanje=PCB::ready;
		Scheduler::put(stari->pcb);
		delete stari;
	}

	running->prviblock=NULL;
	running->poslblock=NULL;
	unlock
	dispatch();*/
}

void PCB::dodaj(PCB* p){
	PCBList *novi=new PCBList(p);

	if (prvi==NULL)
		prvi=novi;
	else
		posl->sled=novi;
	posl=novi;
}

void PCB::ukloni(ID id){
	PCBList *tek=prvi,*pred=NULL;

	while ((tek!=NULL)&&(tek->pcb->id!=id)) {
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

PCB* PCB::dohvatiPCB(ID id){
	PCBList *tek=prvi;

		while ((tek!=NULL)&&(tek->pcb->id!=id))
			tek=tek->sled;

		if (tek==NULL)
			return NULL;
		else
			return tek->pcb;
}

PCB::~PCB(){

	delete [] stek;
	//myThread=NULL;
	ukloni(this->id);

}


