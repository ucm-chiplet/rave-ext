#include "rave_user_events.h"

#define work() for(int i=0; i<100; ++i) asm volatile("nop\n");

int main(int argc, char * argv[]){

	rave_begin_region("r1");
		work()

		rave_begin_region("r2");
			work()
		rave_end_region("r2");
	
		rave_begin_region("r3");
			work()
			rave_begin_region("r4");
				work()
			rave_end_region("r4");
		rave_end_region("r3");
	
		rave_begin_region("r5");
			work()
			rave_begin_region("r4");
				work()
			rave_end_region("r4");

			rave_begin_region("r6");
				work()
			rave_end_region("r6");

			rave_begin_region("r7");
				work()
			rave_end_region("r7");
		rave_end_region("r5");
	
		rave_begin_region("r7");
			work()
		rave_end_region("r7");

	rave_end_region("r1");

	return 0;
}
