#include "pcb.h"
#include "semlist.h"
#include "ksem.h"

KernelSem::KernelSem(int init) : value(init), WaitReturnValue(0) {
	SemaphoreList.add(this);
}

KernelSem::~KernelSem() {
	SemaphoreList.remove(this);
}

int KernelSem::val() {
	return value;
}

void KernelSem::wait(Time maxTimeToWait) {
	if( --value < 0 ) {
		Running->Status = BLOCKED;
		if( !maxTimeToWait )
			WaitReturnValue = 1;
		else
			WaitReturnValue = 0;
		
		WL.Insert(maxTimeToWait, (PCB *) Running);
		UNLOCK
		dispatch();
		
		if( !WaitReturnValue )
			value++;
	}
}

void KernelSem::signal() {
	if( value++ < 0 ) 
		WL.DeleteFirst();
	
	WaitReturnValue = 1;
}
