#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
typedef void interrupt (* Inter )(...);

class KernelEv;

class Event {
public:
 Event (IVTNo ivtNo);
 ~Event ();
 void wait ();
protected:
 friend class KernelEv;
 void signal(); // can call KernelEv
private:
 KernelEv* myImpl;
};

class IVTEntry {
public:
	static KernelEv * events[256];
	static Inter old_routines[256];
	static Inter new_routines[256];
	
	IVTEntry(Inter fun, IVTNo ivtNo) {
		new_routines[ivtNo] = fun;
	}
	
	KernelEv * getEntry(IVTNo i) {
		return events[i];
	}
	
	static void signal(IVTNo);
};

#define PREPAREENTRY(num, old)\
			void interrupt int##num(){\
				IVTEntry::signal(num);\
				if( old )\
					IVTEntry::old_routines[num]();\
			}\
			IVTEntry Interrupt##num( (Inter) int##num, (IVTNo) num );

#endif 