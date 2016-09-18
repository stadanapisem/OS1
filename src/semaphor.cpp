#include "pcb.h"
#include "ksem.h"
#include "semaphor.h"

Semaphore::Semaphore(int init) {
	LOCK
	myImpl = new KernelSem(init);
	UNLOCK
}

Semaphore::~Semaphore() {
	LOCK
	delete myImpl;
	UNLOCK
}

int Semaphore::val() const {
	return myImpl->val();
}

int Semaphore::wait(Time maxTimeToWait) {
	LOCK
	myImpl->wait(maxTimeToWait);
	UNLOCK
	
	return myImpl->WaitReturnValue;
}

void Semaphore::signal() {
	LOCK
	myImpl->signal();
	UNLOCK
}


