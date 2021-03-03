/*
 * kernel.cpp
 *
 *  Created on: May 23, 2018
 *      Author: OS1
 */

#include <iostream.h>
#include <dos.h>
#include "kernel.h"
#include "SCHEDULE.H"
#include "PCB.h"
#include "thread.h"
#include "kerSem.h"
#include "KernelEv.h"

volatile unsigned int ttsp,ttss;
volatile unsigned int pdx,pcx;
extern volatile int lockflag;
extern volatile int prekid;
extern volatile Time brojac;

volatile KerThread* KerThread::runningKernelThread=NULL;


Data *globalData=NULL;

extern volatile unsigned int promena_konteksta;


void interrupt KerThread::SysCall(...){
#ifndef BCC_BLOCK_IGNORE
			asm{
				mov ttsp,sp
				mov ttss,ss
				mov pdx,dx
				mov pcx,cx
			}

			globalData=(Data*) MK_FP(pdx,pcx);
#endif

			//cout<<"usao .\n";
			//asm cli;
			PCB::running->sp=ttsp;
			PCB::running->ss=ttss;


			if (KerThread::runningKernelThread==NULL){
				//cout<<"pravi prvu .\n";
				//asm cli;
				new PCB(globalData->stackSize,globalData->timeSlice,globalData->begin);
			}
			else{
				if ((globalData->ime==threadDelete)&&(globalData->threadID==KerThread::runningKernelThread->myPCB)){
					delete PCB::dohvatiPCB(KerThread::runningKernelThread->myPCB);
				}

				else{
					//cout<<"elseee "<<KerThread::runningKernelThread->myPCB<<".\n";
					//asm cli;
					ttsp=(PCB::dohvatiPCB(KerThread::runningKernelThread->myPCB))->sp;
					ttss=(PCB::dohvatiPCB(KerThread::runningKernelThread->myPCB))->ss;
				}
			}

#ifndef BCC_BLOCK_IGNORE
			asm{
				mov sp, ttsp
				mov ss, ttss
			}
#endif
}



void KerThread::run(){
	while (1){

		//cout<<"whileeeeeee nit\n";
		lockflag=1;
			switch(globalData->ime){
			case threadCreate:
				//cout<<"kreiram nit\n";
				new PCB(globalData->stackSize,globalData->timeSlice,globalData->begin);
				break;
			case threadStart:
				//cout<<"startovana nit\n";
				PCB::dohvatiPCB(globalData->threadID)->start();
				break;
			case threadWaitToComplete:
				PCB::dohvatiPCB(globalData->threadID)->waitToComplete();
				break;
			case threadSleep:
				PCB::sleep(globalData->timeToSleep);
				break;
			case threadDelete:
				delete PCB::dohvatiPCB(globalData->threadID);
				break;
			case semCreate:
				new KernelSem(globalData->init);
				break;
			case semSignal:
				KernelSem::dohvatiSem(globalData->semID)->signal();
				break;
			case semWait:
				globalData->ret=KernelSem::dohvatiSem(globalData->semID)->wait(globalData->toBlock);
				break;
			case semVal:
				globalData->ret=KernelSem::dohvatiSem(globalData->semID)->val();
				break;
			case semDelete:
				delete KernelSem::dohvatiSem(globalData->semID);
				break;
			case eventCreate:
				new KernelEv(globalData->brUlaza);
				break;
			case eventWait:
				KernelEv::dohvatiEvent(globalData->eventID)->wait();
				break;
			case eventDelete:
				delete KernelEv::dohvatiEvent(globalData->eventID);
				break;
			case DISPATCH:
				promena_konteksta=1;
				break;

			}
		lockflag=0;
#ifndef BCC_BLOCK_IGNORE
		asm int 63h;
#endif
	}
}


void interrupt KerThread::SysCallBack(...){
#ifndef BCC_BLOCK_IGNORE
			asm{
				mov ttsp,sp
				mov ttss,ss
			}
#endif

			PCB::dohvatiPCB(KerThread::runningKernelThread->myPCB)->sp=ttsp;
			PCB::dohvatiPCB(KerThread::runningKernelThread->myPCB)->ss=ttss;

			if (PCB::running->stanje!=PCB::ready){
				PCB::running=Scheduler::get();
				brojac=PCB::running->timeSlice;
				promena_konteksta=0;
				prekid=0;
			}
			else{
				if ((promena_konteksta)||(prekid)){
					Scheduler::put((PCB*)PCB::running);
					PCB::running=Scheduler::get();
					brojac=PCB::running->timeSlice;
					promena_konteksta=0;
					prekid=0;
				}
			}

			if(PCB::running==NULL){
				PCB::running=PCB::dohvatiPCB(PCB::idle->myPCB);
				brojac=PCB::running->timeSlice;
			}

			ttsp=PCB::running->sp;
			ttss=PCB::running->ss;

#ifndef BCC_BLOCK_IGNORE
			asm{
				mov sp, ttsp
				mov ss, ttss
			}
#endif

}
