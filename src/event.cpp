#include "pcb.h"
#include "kevent.h"
#include "event.h"


Event::Event(IVTNo ivtNo) {
	LOCK
	myImpl = new KernelEv((PCB *) Running, ivtNo);
	UNLOCK
}

Event::~Event() {
	delete myImpl;
}

void Event::wait() {
	LOCK
	myImpl->wait();
	UNLOCK
}

void Event::signal() {
	LOCK
	myImpl->signal();
	UNLOCK
}