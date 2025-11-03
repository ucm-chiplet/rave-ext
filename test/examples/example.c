#include "rave_user_events.h"

__attribute__ ((noinline))
void initialize(int N, double *A, double *B, double *C){
	rave_begin_region("ini_A");
	for(int i=0; i<N; ++i){
		A[i] = i;
	}
	rave_end_region("ini_A");

	rave_begin_region("ini_B");
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		B[i] = 2.5;
	}
	rave_end_region("ini_B");

	rave_stop_trace();

	rave_begin_region("ini_C");
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		C[i] = -i;
	}
	rave_end_region("ini_C");

	rave_start_trace();
}

__attribute__ ((noinline))
void compute(int N, double *A, double *B, double *C){
	rave_begin_region("arith_vec");
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		A[i] -= B[i]*0.2 + 0.5*C[i];
	}
	rave_end_region("arith_vec");

	rave_begin_region("if_vec");
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		if (A[i] > 0.5) C[i] += A[i]*0.2;
	}
	rave_end_region("if_vec");
}


int main(){
	int N = 256*10 + 13;
	rave_restart_trace();

	double A[N];
	double B[N];
	double C[N];

	rave_begin_region("initialization");
	initialize(N,A,B,C);
	rave_end_region("initialization");

	rave_begin_region("compute");
	compute(N,A,B,C);
	rave_end_region("compute");

	volatile double noopt = C[0];
}
