/*
 * main.cpp
 *
 *  Created on: May 12, 2018
 *      Author: OS1
 */
#include "PCB.h"
#include <iostream.h>
extern int userMain(int argc, char* argv[]);
extern volatile int lockflag;
extern volatile int lockbr;
extern volatile int prekid;

int main(int argc, char** argv){
	lock
		cout<<"pocetak \n";
	unlock
	PCB::inic();
	lock
	cout<<"inic \n";
	unlock
	int ret=userMain(argc,argv);
	lock
	cout<<"uradio \n";
	unlock
	PCB::restore();
	return ret;
}

