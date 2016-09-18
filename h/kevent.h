#ifndef _KEVENT_H_
#define _KEVENT_H_

#include "event.h"

class KernelEv {
public:
	IVTNo ivtNo;
	PCB * myPCB;
	
	KernelEv(PCB *, IVTNo);
	~KernelEv();
	
	void wait();
	void signal();
	
	void restore_entry();
	void override_entry();
};



#endif