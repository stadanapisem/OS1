#ifndef _SEMLIST_H_
#define _SEMLIST_H_

#include <stdio.h>

class KernelSem;

class SemList {
	
	struct Elem {
		KernelSem * Data;
		Elem * nxt;
		
		Elem(KernelSem * k, Elem * n = NULL) : Data(k), nxt(n) {}
	};
	
	Elem * head;
	
public:
	SemList() {
		head = NULL;
	}
	~SemList();
	
	void add(KernelSem *);
	void remove(KernelSem *);
	void unblock();
};

extern SemList SemaphoreList;

#endif