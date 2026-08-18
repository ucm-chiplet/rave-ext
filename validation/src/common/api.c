#include "rave_user_events.h"

int main(int argc, char * argv[]){

	const long N = 100;
	int A[N];
	int accum = 0;

	rave_begin_region("region_erased");
	for(int i=0; i<N; ++i) A[i]=i;
	rave_end_region("region_erased");

	rave_restart_trace();

	rave_begin_region("region1");
	for(int i=0; i<N; ++i) accum += A[i];
	rave_end_region("region1");

	rave_disable_trace();

	rave_begin_region("region_notrace");
	rave_event_and_value(13,13);
	for(int i=0; i<N; ++i) accum += A[i];
	rave_end_region("region_notrace");

	rave_enable_trace();

	rave_begin_region("region2");
	for(int i=0; i<N; ++i) accum += A[i];
	rave_end_region("region2");

	rave_disable_regions();

	rave_begin_region("region_disabled");
	for(int i=0; i<N; ++i) accum += A[i];
	rave_end_region("region_disabled");

	rave_enable_regions();

	rave_begin_region("region3");
	for(int i=0; i<N; ++i) accum += A[i];
	rave_end_region("region3");

	rave_name_event(42,"forty-two");
	rave_name_value(42,11,"eleven"); 
	rave_event_and_value(42,11);

	volatile int noopt = accum; 
}
