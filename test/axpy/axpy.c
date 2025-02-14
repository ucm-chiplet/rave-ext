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
	rave_name_event(1000,"code_region");
	rave_name_value(1000,0,"End");
	rave_name_value(1000,1,"ini_simd");
	rave_name_value(1000,2,"ini_omp");
	rave_name_value(1000,3,"axpy");
	rave_name_value(1000,4,"validate");
	rave_restart_trace();

	int N = 1<<20;
	double * Y = (double *)malloc(sizeof(double)*N);
	double * X = (double *)malloc(sizeof(double)*N);
	double alpha = 42;

	rave_event_and_value(1000,1)
	#pragma omp simd
	for(int i=0; i<N; ++i){
		Y[i]=0;
	}
	rave_event_and_value(1000,2)
	#pragma omp parallel for
	for(int i=0; i<N; ++i){
		X[i]=(double)(i%32);
	}
	rave_event_and_value(1000,3)
	#pragma omp parallel for simd
	for(int i=0; i<N; ++i){
		Y[i] += X[i] * alpha;
	}
	rave_event_and_value(1000,0)

	#pragma omp parallel
	{
	#pragma omp single
	{
	int batch = N/8;
	for(int i=0; i<N; i+=batch){
		#pragma omp task 
		{
		rave_event_and_value(1000,4)
		validate(&Y[i], &X[i], alpha, batch);
		rave_event_and_value(1000,0)
		}
	}
	}
	}
	
	volatile double y = Y[0];
	free(X);
	free(Y);
	return 0;
}
