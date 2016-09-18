#include "waitl.h"
#include "SCHEDULE.h"

WaitList::~WaitList() {
	LOCK
	while( head ) {
		Elem * del = head;
		head = head->nxt;
		
		delete del;
	}
	UNLOCK
}

void WaitList::Insert(unsigned t, PCB * p) {
	LOCK
	if( t ) {
		Elem * last = 0;
		
		for( Elem * i = head; i && i->Time <= t; i = i->nxt )
			t -= i->Time, last = i;
		
		
		Elem * tmp = new Elem(t, p);
		
		if( !last ) {
			tmp->nxt = head;
			head = tmp;
		} else {
			tmp->nxt = last->nxt;
			last->nxt = tmp;
		}
	} else {
		Elem * tmp = new Elem(t, p, head);
		head = tmp;
	}
	UNLOCK
}

void WaitList::Delete() {
	
	Elem * last = NULL, * tmp = head;
	while( tmp && !tmp->Time ) {
		last = tmp;
		tmp = tmp->nxt;
	}
	
	if( tmp ) {
		tmp->Time--;
		
		while( tmp && tmp->Time == 0 ) {
			Elem * del = tmp;
			tmp = tmp->nxt;
			if( !last ) 
				head = head->nxt;
			else last->nxt = tmp;
			
			del->pcb->Status = READY;
			Scheduler::put(del->pcb);
			delete del;
		}
	}
}

void WaitList::DeleteFirst() {
	Elem * del = head;
	
	if( head->nxt ) 
		head->nxt->Time += head->Time;
	
	
	head->pcb->Status = READY;
	
	Scheduler::put(head->pcb);
	head = head->nxt;
	delete del;
}

void WaitList::DeleteByID(int id) {
	LOCK
	for( Elem * i = head, * last = 0; i; i = i->nxt ) {
			if( i->pcb->ID == id ) {
				Elem * del = i;
				if( !last )
					head = i->nxt;
				else last->nxt = i->nxt;
				
				if( del->nxt )
					del->nxt->Time += del->Time;
				
				delete del;
				UNLOCK
				return;
			}
	}	
	UNLOCK
}

