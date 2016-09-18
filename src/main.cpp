#include <stdio.h>
#include "timer.h"
#include "thread.h"
#include "pcb.h"
#include "pcblist.h"
#include "schedule.h"
#include "semaphor.h"
#include "kevent.h"

void idle_function() {
	for( ;; ) ;
}

PCB * main_pcb = new PCB(0, 0, 0);
PCB * idle = new PCB(idle_function);

extern int userMain(int argc, char ** argv);
int ReturnValue;

class MainThread : public Thread {
	int argc;
	char ** argv;
	
public:
	
	MainThread(int argc, char ** argv) : Thread(), argv(argv), argc(argc) {}
	~MainThread() {
		waitToComplete();
	}
	
	void run() {
		ReturnValue = userMain(argc, argv);
	}
};

int main(int argc, char ** argv) {
	InitializeTimer();
	Running = main_pcb;
	
	MainThread * main = new MainThread(argc, argv);
	main->start();
	
	delete main;
	
	RestoreTimer();
	
	return ReturnValue;
}
