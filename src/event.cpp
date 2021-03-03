/*
 * event.cpp
 *
 *  Created on: May 18, 2018
 *      Author: OS1
 */
#include <iostream.h>
#include <dos.h>
#include "event.h"
#include "KernelEv.h"
#include "kernel.h"

extern Data *globalData;

Event::Event(IVTNo ivtNo){

	unsigned int d,c;
		Data *params=new Data();
		params->ime=eventCreate;
		params->brUlaza=ivtNo;

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

void Event::signal(){ // ZASTO OVO NIJE PREKO SYS_CALL?
	KernelEv::dohvatiEvent(myImpl)->signal();//proveri ovo!!!
}


void Event::wait(){
	unsigned int d,c;
		Data *params=new Data();
		params->ime=eventWait;
		params->eventID=myImpl;

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

Event::~Event(){

	unsigned int d,c;
			Data *params=new Data();
			params->ime=eventDelete;
			params->eventID=myImpl;

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
