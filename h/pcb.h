#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "pcblist.h"
#include "semaphor.h"

extern volatile int lockFlag, context_switch_request;
extern volatile PCB * Running;

#define LOCK lockFlag = 1;
#define UNLOCK lockFlag = 0;\
	if(context_switch_request) dispatch();

typedef enum {
	IDLE, INIT, RUNNING, BLOCKED, READY, FINISHED
} Statuses;

class PCB {
	static int _ID;
	
public:
	Statuses Status;
	int ID;
	unsigned SS, SP, BP, * Stack;
	Thread * myThread;
	Time timeSlice;
	char Unlimited;
	StackSize stackSize;
	
	PCBList SignalTo;
	
	int ChildrenNo;
	Semaphore waitForChildren;
	char isForkChild;
	int ForkReturnValue;
	Thread * parentThread;
	static void interrupt fork(); 
	
	PCB(StackSize, Time, Thread*);
	PCB(void (*fun) ());
	~PCB();
	
	static void onRun();
	static void onFinish();
};

#endif