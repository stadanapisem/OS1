#ifndef _WAITL_H_
#define _WAITL_H_

#include <stdio.h>
#include "pcb.h"

class WaitList {
	
	struct Elem {
		unsigned int Time;
		PCB * pcb;
		
		Elem * nxt;
		
		Elem(unsigned int t, PCB * p, Elem * n = 0) : Time(t), pcb(p), nxt(n) {}
	};
	
public:
	
	Elem * head;
	
	WaitList() : head(0) {}
	~WaitList();
	
	void Insert(unsigned, PCB *);
	void Delete();
	void DeleteFirst();
	void DeleteByID(int);
	int Empty() {
		return !head;
	}
};

#endif