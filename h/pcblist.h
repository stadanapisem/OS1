#ifndef _PCBLIST_H_
#define _PCBLIST_H_

#include <stdio.h>
#include <iostream.h>

class PCB;
class PCBList {

	struct Elem {
		PCB * Data;
		Elem * nxt;

		Elem(PCB * p, Elem * n = NULL) {
			Data = p;
			nxt = n;
		}
	};

	Elem * first;

public:

	PCBList() {
		//LOCK
		first = NULL;
		//UNLOCK
	}

	PCB * GetPCB(int);
	PCB * Front();
	void Put(PCB *);
	void Remove(int);
	void NotifyAll();
	
	int Empty() {
		return first ? 0 : 1;
	}

	~PCBList();
};

extern PCBList List;

#endif /* PCBLIST_H_ */
