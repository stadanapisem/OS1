#include <dos.h>
#include "pcb.h"
#include "thread.h"
#include "pcblist.h"

volatile PCB * Running;
volatile int lockFlag, context_switch_request;

int PCB::_ID = 0;

PCB::PCB(StackSize stackSiz, Time timeSlice, Thread * myThread) : ID(++_ID), stackSize(stackSiz), timeSlice(timeSlice), myThread(myThread), Unlimited(0), waitForChildren(0), parentThread(0), ChildrenNo(0), Status(INIT) {
	if(stackSize < 65536 && stackSize > 0) {
		stackSize /= sizeof (unsigned);
		Stack = new unsigned[stackSize];
		
		Stack[stackSize - 1] = 0x200;
		Stack[stackSize - 2] = FP_SEG(PCB::onRun);
		Stack[stackSize - 3] = FP_OFF(PCB::onRun);
		Stack[stackSize - 12] = -1;
		
		SP = BP = FP_OFF(Stack + stackSize - 12);
		SS = FP_SEG(Stack + stackSize - 12);
		
		List.Put(this);
		
	} else Stack = 0;
	
	if( !timeSlice )
		Unlimited = 1;
	
}

PCB::PCB( void (*fun) () ) : ID(++_ID), stackSize(128), timeSlice(1), myThread(0), Unlimited(0), Status(IDLE) {
	LOCK
	stackSize /= 2;
	Stack = new unsigned[stackSize];
	Stack[stackSize - 1] = 0x200;
	Stack[stackSize - 2] = FP_SEG(fun);
	Stack[stackSize - 3] = FP_OFF(fun);
	
	SP = BP = FP_OFF(Stack + stackSize - 12);
	SS = FP_SEG(Stack + stackSize - 12);
	UNLOCK
}

PCB::~PCB() {
	LOCK
	delete [] Stack;
	UNLOCK;
}

void PCB::onRun() {
	Running->myThread->run();
	
	onFinish();
	dispatch();
}

void PCB::onFinish() {
	LOCK
	Running->Status = FINISHED;
	
	if( !((PCB *) Running)->SignalTo.Empty() )
		((PCB *)Running)->SignalTo.NotifyAll();
	
	if( Running->parentThread )
		Running->parentThread->myPCB->waitForChildren.signal();
	
	List.Remove(Running->ID);
	
	UNLOCK
}

extern Thread * ChildThread;
static unsigned tbp, tss, chloff, paroff, newparoff, newchloff;

void interrupt PCB::fork() {
	
	delete[] ChildThread->myPCB->Stack; // Da bi se napravio stack prave velicine, a ne default velicine
	ChildThread->myPCB->stackSize = Running->stackSize;
	ChildThread->myPCB->Stack = new unsigned[Running->stackSize];
	
	ChildThread->myPCB->timeSlice = Running->timeSlice;
	ChildThread->myPCB->Unlimited = Running->Unlimited;
	ChildThread->myPCB->parentThread = Running->myThread;
	ChildThread->myPCB->isForkChild = 1;
	Running->ForkReturnValue = ChildThread->myPCB->ID;
	Running->ChildrenNo ++;
	
	memcpy(ChildThread->myPCB->Stack, Running->Stack, Running->stackSize * sizeof(unsigned));
	
	asm {
		mov tbp, bp
		mov tss, ss
	}
	
	ChildThread->myPCB->SS = FP_SEG(ChildThread->myPCB->Stack);
	paroff = FP_OFF(Running->Stack);
	chloff = FP_OFF(ChildThread->myPCB->Stack);
	ChildThread->myPCB->BP = ChildThread->myPCB->SP = tbp - paroff + chloff;
	
	paroff = tbp;
	chloff = ChildThread->myPCB->BP;
	
	for( ;; ) {
		
		newparoff = *((unsigned *) MK_FP(tss, paroff));
		newchloff = *((unsigned *) MK_FP(ChildThread->myPCB->SS, chloff));
		
		if( newparoff == -1 ) {
			newchloff = -1;
			break;
		}
		
		newchloff = newparoff - paroff + chloff;
		
		paroff = newparoff;
		chloff = newchloff;
	}
	
	ChildThread->start();
}





