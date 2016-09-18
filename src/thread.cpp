#include <dos.h>
#include "thread.h"
#include "pcb.h"
#include "schedule.h"
#include "pcblist.h"

Thread::Thread(StackSize stackSize, Time timeSlice) {
	LOCK
	myPCB = new PCB(stackSize, timeSlice, this);
	UNLOCK
}

Thread::~Thread() {
	LOCK
	waitToComplete();
	delete myPCB;
	UNLOCK
}

void Thread::start() {
	LOCK
	myPCB->Status = READY;
	Scheduler::put(myPCB);
	UNLOCK
}

void Thread::waitToComplete() {
	LOCK
	
	if( myPCB != (PCB *) Running && myPCB->Status != FINISHED ) {
		myPCB->SignalTo.Put((PCB *) Running);
		Running->Status = BLOCKED;
		
		UNLOCK
		dispatch();
	}
	
	UNLOCK
}

ID Thread::getId() {
	return myPCB->ID;
}

ID Thread::getRunningId() {
	return Running->ID;
}

Thread * Thread::getThreadById(ID id) {
	return List.GetPCB(id)->myThread;
}

Thread * Thread::clone() const {
	return new Thread();
}

void Thread::waitForForkChildren() {
	while( Running->ChildrenNo > 0 ) {
		((PCB *) Running)->waitForChildren.wait(0);
		Running->ChildrenNo--;
	}	
}

void Thread::exit() {
	
	if( Running->Status != FINISHED )
		PCB::onFinish();
	
	dispatch();
}

Thread * ChildThread = 0;

ID Thread::fork() {
	LOCK
	
	if( Running->isForkChild )
		Running->isForkChild = 0;
	
	ChildThread = 0;
	ChildThread = Running->myThread->clone();
	
	if( !ChildThread ) {
		UNLOCK
		return -1;
	}
	
	PCB::fork();
	
	UNLOCK
	return Running->isForkChild ? 0 : Running->ForkReturnValue;
}

void dispatch() {
	//LOCK
	context_switch_request = 1;
	asm int 8h;
	//UNLOCK
}