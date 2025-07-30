#include "rave_user_events.h"
#include <stdlib.h>
#include <stdio.h>

__attribute__((noinline))
void validate(double * Y, double * X, double alpha, int N){
	double err=0;
	#pragma omp simd
	for(int i=0; i<N; ++i){
		double diff = (Y[i] - X[i]*alpha);
		err += diff*diff;
	}
	if (err > 1e-3) printf("Error!\n");
}

int main(){
	long gvl;
	rave_restart_trace();

	int N = 1<<20;
	double * Y = (double *)malloc(sizeof(double)*N);
	double * X = (double *)malloc(sizeof(double)*N);
	double alpha = 42;

	rave_begin_region("ini-simd");
	#pragma omp simd
	for(int i=0; i<N; ++i){
		Y[i]=0;
	}
	rave_end_region("ini-simd");
	rave_begin_region("ini-omp");
	#pragma omp parallel for
	for(int i=0; i<N; ++i){
		X[i]=(double)(i%32);
	}
	rave_end_region("ini-omp");
	rave_begin_region("axpy");
	#pragma omp parallel for simd
	for(int i=0; i<N; ++i){
		Y[i] += X[i] * alpha;
	}
	rave_end_region("axpy");

	#pragma omp parallel
	{
	#pragma omp single
	{
	int batch = N/8;
	for(int i=0; i<N; i+=batch){
		#pragma omp task 
		{
		rave_begin_region("validate");
		validate(&Y[i], &X[i], alpha, batch);
		rave_end_region("validate");
		}
	}
	}
	}
	
	volatile double y = Y[0];
	free(X);
	free(Y);
	return 0;
}
