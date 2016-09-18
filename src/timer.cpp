#include <dos.h>
#include <stdio.h>
#include <iostream.h>
#include "pcb.h"
#include "scheduler.h"
#include "pcblist.h"
#include "semlist.h"

static unsigned oldTimerSEG, oldTimerOFF;
volatile unsigned TimeSlice;
static unsigned tsp, tss, tbp;
extern PCB * idle;

extern void tick();

void interrupt timer() {
	//printf("%d %u %d\n", List.Empty(), Running->ID, Running->Status == BLOCKED);
	if( !context_switch_request && !Running->Unlimited ) {
		TimeSlice--;
	}
	
	if( !context_switch_request ) {
		tick();
		asm int 60h
	}
	
	if( !context_switch_request )
		SemaphoreList.unblock();

	if( ( (!TimeSlice && !Running->Unlimited) || context_switch_request) ) {
		if( !lockFlag ) {
			context_switch_request = 0;

			asm {
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			Running->SS = tss;
			Running->SP = tsp;
			Running->BP = tbp;

			if( Running->Status != IDLE && Running->Status != FINISHED && Running->Status != BLOCKED ) { // IF NOT IDLE, FINISHED, BLOCKED
				Scheduler::put( (PCB*) Running);
				Running->Status = READY;
			}

			Running = Scheduler::get();
			
			if( Running )
				Running->Status = RUNNING;
			else if( !Running && !List.Empty() )
				Running = idle;

			tsp = Running->SP;
			tss = Running->SS;
			tbp = Running->BP;
			TimeSlice = Running->timeSlice;


			asm {
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
		} else
			context_switch_request = 1;
	}
}


void InitializeTimer() {
	LOCK

	asm {
		push es
		push ax

		mov ax, 0
		mov es, ax

		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
	}

	UNLOCK
}

void RestoreTimer() {
	LOCK

	asm {
		push es
		push ax

		mov ax, 0
		mov es, ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
	}

	UNLOCK
}
