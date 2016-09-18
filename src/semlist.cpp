#include "ksem.h"
#include "pcb.h"
#include "semlist.h"

SemList SemaphoreList;

SemList::~SemList() {
	LOCK
	while( head ) {
		Elem * del = head;
		head = head->nxt;
		delete del;
	}
	UNLOCK
}

void SemList::add(KernelSem * k) {
	LOCK
	
	Elem * tmp = new Elem(k, head);
	head = tmp;
	UNLOCK
}

void SemList::remove(KernelSem * k) {
	LOCK
	Elem * last = NULL;
	
	for( Elem * i = head; i; i = i->nxt ) {
		if( i->Data == k ) {
			
			if( !last )
				head = head->nxt;
			else last->nxt = i->nxt;
			
			delete i;
			
			UNLOCK
			return ;
		}
		
		last = i;
	}
	UNLOCK
}

void SemList::unblock() {
	LOCK
	for( Elem * i = head; i; i = i->nxt )
		i->Data->WL.Delete();
	
	UNLOCK
}


