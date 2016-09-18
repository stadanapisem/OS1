#ifndef _KSEM_H_
#define _KSEM_H_

#include "semaphor.h"
#include "waitl.h"

class KernelSem {
	
public:
	KernelSem(int);
	~KernelSem();
	
	void wait(Time);
	void signal();
	int val();
	int value, WaitReturnValue;
	WaitList WL;
};

#endif