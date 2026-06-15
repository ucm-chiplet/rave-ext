#include <stdio.h>
#include "omp.h"
int main(){
	omp_set_num_threads(4);
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Hello, I'm %d\n",id); 
		double A[1024*4];
		for(int j=0; j<1024*(id+1); ++j){
			A[j] = id;
		}
		volatile double noopt = A[id];
	}
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Bye, I'm %d\n",id); 
		double A[1024*4];
		for(int j=0; j<1024*(4-id); ++j){
			A[j] = id;
		}
		volatile double noopt = A[id];
	}
	return 0;
}
