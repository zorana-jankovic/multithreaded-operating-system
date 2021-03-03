/*
 * semaphor.cpp
 *
 *  Created on: May 13, 2018
 *      Author: OS1
 */
#include"semaphor.h"
#include "kernel.h"
#include <dos.h>




Semaphore::Semaphore(int init){
	unsigned int d,c;
	Data* params=new Data();
	params->ime=semCreate;
	params->init=init;

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


	myImpl=params->ret;
	delete params;

}


void Semaphore::signal(){
	unsigned int d,c;
	Data* params=new Data();
	params->ime=semSignal;
	params->semID=myImpl;

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

int Semaphore::wait(int toBlock){
	unsigned int d,c;
	Data* params=new Data();
	params->ime=semWait;
	params->semID=myImpl;
	params->toBlock=toBlock;

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

	return params->ret;
	delete params;
}



int Semaphore::val()const{
	unsigned int d,c;
	Data* params=new Data();
	params->ime=semVal;
	params->semID=myImpl;

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

	return params->ret;
	delete params;
}

Semaphore::~Semaphore(){
	unsigned int d,c;
	Data* params=new Data();
	params->ime=semDelete;
	params->semID=myImpl;

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
