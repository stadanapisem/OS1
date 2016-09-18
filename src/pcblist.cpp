#include "pcblist.h"
#include "SCHEDULER.h"
#include "pcb.h"

PCBList List;

PCBList::~PCBList() {
	LOCK
	while( first ) {

		Elem * del = first;
		first = first->nxt;

		delete(del);
	}
	UNLOCK
}


void PCBList::Put(PCB * pcb) {
	LOCK
	Elem * tmp = new Elem(pcb);
	
	if( first )
		tmp->nxt = first;
	
	first = tmp;
	
	UNLOCK
}

PCB * PCBList::GetPCB(int id) {
	LOCK
	for(Elem * i = first; i; i = i->nxt)
		if(i->Data->ID == id) {
			UNLOCK
			return i->Data;
		}
	
	UNLOCK
	return NULL;
}

PCB * PCBList::Front() {
	LOCK
	PCB * ret = first->Data;
	Elem * del = first;

	first = first->nxt;
	
	if( !first ) {
		first = 0;
	}
	
	delete del;
	UNLOCK
	return ret;
}

void PCBList::Remove(int id) {
	LOCK
	Elem * pre = NULL;

	for(Elem * i = first; i; i = i->nxt){
		if(i->Data->ID == id) {
			if(!pre)
				first = first->nxt;
			else pre->nxt = i->nxt;

			delete i;
			UNLOCK
			return ;
		}
		pre = i;
	}
	UNLOCK
}

void PCBList::NotifyAll() {
	
	while( first ) {
		Elem * tmp = first;
		tmp->Data->Status = READY;
		
		Scheduler::put(tmp->Data);
		first = first->nxt;
		delete tmp;
	}
}
