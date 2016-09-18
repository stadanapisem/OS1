#include <dos.h>
#include "pcb.h"
#include "schedule.h"
#include "kevent.h"

KernelEv * IVTEntry::events[256];
Inter IVTEntry::old_routines[256];
Inter IVTEntry::new_routines[256];

KernelEv::KernelEv(PCB * Owner, IVTNo num) : myPCB(Owner), ivtNo(num) {
	IVTEntry::events[ivtNo] = this;
	override_entry();
}

KernelEv::~KernelEv() {
	IVTEntry::events[ivtNo] = 0;
	restore_entry();
}

void KernelEv::restore_entry() {
	setvect(ivtNo, IVTEntry::old_routines[ivtNo]);
}

void KernelEv::override_entry() {
	IVTEntry::old_routines[ivtNo] = getvect(ivtNo);
	setvect(ivtNo, IVTEntry::new_routines[ivtNo]);
}

void KernelEv::signal() {
	myPCB->Status = READY;
	Scheduler::put(myPCB);
	dispatch();
}

void KernelEv::wait() {
	if( (PCB *) Running != myPCB ) 
		return ;
	
	myPCB->Status = BLOCKED;
	
	UNLOCK
	dispatch();
}

void IVTEntry::signal(IVTNo num) {
	events[num]->signal();
}