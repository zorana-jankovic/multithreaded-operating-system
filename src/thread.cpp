/*
 * thread.cpp
 *
 *  Created on: May 10, 2018
 *      Author: OS1
 */
#include "thread.h"
#include "PCB.h"
#include "kernel.h"
#include "SCHEDULE.h"
#include <dos.h>


extern volatile unsigned int promena_konteksta;

Thread::Thread(StackSize stackSize,Time timeSlice){
	unsigned int d,c;
	//myPCB=new PCB(stackSize,timeSlice,this);

	Data *params=new Data();
	params->ime=threadCreate;
	params->stackSize=stackSize;
	params->timeSlice=timeSlice;
	params->begin=this;

#ifndef BCC_BLOCK_IGNORE
	asm{
		push dx
		push cx
	}

	d=FP_SEG(params);
	c=FP_OFF(params);

	asm{
		mov dx,d
		mov cx,c



		int 61h

		pop cx
		pop dx
	}
#endif

	myPCB=params->ret;

	delete params;
}

void Thread::start(){
	unsigned int d,c;
	Data *params=new Data();
	params->ime=threadStart;
	params->threadID=myPCB;

#ifndef BCC_BLOCK_IGNORE
	asm{
		push dx
		push cx
	}

	d=FP_SEG(params);
	c=FP_OFF(params);

	asm{
		mov dx,d
		mov cx,c



	}
	KerThread::SysCall();

	asm{
		pop cx
		pop dx
	}
#endif

	delete params;
}

void Thread::waitToComplete(){
	unsigned int d,c;
		Data *params=new Data();
		params->ime=threadWaitToComplete;
		params->threadID=myPCB;

	#ifndef BCC_BLOCK_IGNORE
		asm{
			push dx
			push cx
		}

		d=FP_SEG(params);
		c=FP_OFF(params);

		asm{
			mov dx,d
			mov cx,c

			int 61h;

			pop cx
			pop dx
		}
	#endif

		delete params;
}

void Thread::sleep(Time timeToSleep){
	unsigned int d,c;
			Data *params=new Data();
			params->ime=threadSleep;
			//params->threadID=PCB::running->id;
			params->timeToSleep=timeToSleep;

		#ifndef BCC_BLOCK_IGNORE
			asm{
				push dx
				push cx
			}

			d=FP_SEG(params);
			c=FP_OFF(params);

			asm{
				mov dx,d
				mov cx,c

				int 61h;

				pop cx
				pop dx
			}
		#endif

			delete params;
}

Thread::~Thread(){
	unsigned int d,c;
			Data *params=new Data();
			params->ime=threadWaitToComplete;
			params->threadID=myPCB;

		#ifndef BCC_BLOCK_IGNORE
			asm{
				push dx
				push cx
			}

			d=FP_SEG(params);
			c=FP_OFF(params);

			asm{
				mov dx,d
				mov cx,c

				int 61h;

				pop cx
				pop dx
			}
		#endif

	params->ime=threadDelete;
	params->threadID=myPCB;

#ifndef BCC_BLOCK_IGNORE
			asm{
				push dx
				push cx
			}

			d=FP_SEG(params);
			c=FP_OFF(params);

			asm{
				mov dx,d
				mov cx,c

				int 61h;

				pop cx
				pop dx
			}
		#endif

			delete params;

}


void dispatch(){

	unsigned int d,c;
				Data *params=new Data();
				params->ime=DISPATCH;
			#ifndef BCC_BLOCK_IGNORE
				asm{
					push dx
					push cx
				}

				d=FP_SEG(params);
				c=FP_OFF(params);

				asm{
					mov dx,word ptr d
					mov cx,word ptr c

					int 61h;

					pop cx
					pop dx
				}
			#endif

				delete params;


	//promena_konteksta=1;
	//PCB::timer();
}



void Thread::wrapper(Thread* running)
{

	//cout<<"usao u wraper\n";
	running->run();

	PCB::running->stanje=PCB::over;
#ifndef BCC_BLOCK_IGNORE
	asm cli;
#endif

	PCBList* tek=PCB::running->prviblock,*stari;
	while(tek!=NULL){
		stari=tek;
		tek=tek->sled;
		stari->pcb->stanje=PCB::ready;
		Scheduler::put(stari->pcb);
		delete stari;
	}

	PCB::running->prviblock=NULL;
	PCB::running->poslblock=NULL;

#ifndef BCC_BLOCK_IGNORE
	asm sti;
#endif

	//cout<<"izasao iz wrapper-a \n";
	dispatch();
}
