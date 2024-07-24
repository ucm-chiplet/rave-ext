#include "rave_user_events.h"

int main(){

	int N = 256*10 + 13;
	rave_name_event(1001,"flops");
	rave_name_event(1000,"code_region");
	rave_name_value(1000,0,"End");
	rave_name_value(1000,1,"ini_A");
	rave_name_value(1000,2,"ini_B");
	rave_name_value(1000,3,"ini_C");
	rave_name_value(1000,4,"arith_vec");
	rave_name_value(1000,5,"if_vec");

	rave_restart_trace();

	double A[N];
	double B[N];
	double C[N];

	rave_event_and_value(1000,1)
	for(int i=0; i<N; ++i){
		A[i] = i;
	}
	rave_event_and_value(1000,0)

	rave_event_and_value(1000,2)
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		B[i] = 2.5;
	}
	rave_event_and_value(1000,0)

	rave_stop_trace();

	rave_event_and_value(1000,3)
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		C[i] = -i;
	}
	rave_event_and_value(1000,0)

	rave_start_trace();

	rave_event_and_value(1000,4)
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		A[i] -= B[i]*0.2 + 0.5*C[i];
	}
	rave_event_and_value(1000,0)

	rave_event_and_value(1000,5)
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		if (A[i] > 0.5) C[i] += A[i]*0.2;
	}
	rave_event_and_value(1000,0)
	volatile double noopt = C[0];
}
